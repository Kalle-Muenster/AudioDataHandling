//css_ref linkage\ControlledValues.dll
//css_ref linkage\WaveFileHandling.dll
//css_ref linkage\StdStreams.dll
//css_co /unsafe /D:SCRIPT

#if !SCRIPT
#define COMPILE
#endif

using System;
using System.Reflection;
using System.Collections.Generic;
using System.Threading.Tasks;

using Stepflow;
using Stepflow.Controller;
using Stepflow.Audio;
using Stepflow.Audio.FrameTypes;
using Stepflow.Audio.FileIO;
using Consola;
using Std = Consola.StdStream;

namespace Stepflow
{
    namespace Audio
    {
        public class ToneGenerator
        {
            private enum FORM : uint
            {
                sin = (uint)ControlMode.Sinus,
                saw = (uint)ControlMode.Cycle,
                pls = (uint)ControlMode.Pulse,
                tri = (uint)ControlMode.Stack
            };

            private static List<Controlled.Float64>[] osc;
            private static List<Controlled.Float64>[] lfo;
#if EXPERIMENTAL
            private static Controlled.Float64[] eq3;
            private static IntPtr[][] eqGain;
#endif
            private static Panorama[][] mixer;
            private static double[][] freqs;
            private static double[][][] amps;
            private static double pinDummy;
            private static bool logging = false;
            private static System.IO.StreamReader tonscript = null;
            private static string[] synthetizationParameters = null;
            private static int synthetizationParameterCount = 0;
            private static string ReadNextParameter()
            {
                string nextArg;
                if ( synthetizationParameters != null ) {
                    while( (nextArg = synthetizationParameters[++synthetizationParameterCount]).StartsWith("#") )
                        Std.Out.WriteLine( nextArg );
                    Std.Out.WriteLine( nextArg );
                } else if ( tonscript != null ) {
                    while( (nextArg = tonscript.ReadLine()).StartsWith("#") )
                        Std.Out.WriteLine( nextArg );
                    Std.Out.WriteLine( nextArg );
                } else {
                    nextArg = Std.Inp.ReadLine();
                    if( logging )
                        Std.Out.Log.WriteLine( nextArg );
                } return nextArg;
            }
            private static double[][] readSeveralFloats( string input )
            {
                return readSeveralFloats(0.5,input,4);
            }
            private static double[][] readSeveralFloats( double def, string input )
            {
                if(def==1) def = 1.0 - double.Epsilon;
                if(def==0) def = double.Epsilon;

                if(input.Length==0)
                    return new double[2][] { new double[]{def,1.0-def},new double[]{def,def} };
                return readSeveralFloats(def,input,4);
            }
            private static double[][] readSeveralFloats(string input, int max_count)
            {
                return readSeveralFloats(0.5,input,max_count);
            }
            private static double[][] readSeveralFloats(double def, string input, int max_count)
            {
                char level = '\0';
                if (max_count > 1)
                    level = input.Contains("~")
                    ? '~' : input.Contains("x")
                    ? 'x' : '\0';

                switch (level)
                {
                    case '~': {
                        string[] inputs = input.Split(level);
                        if (max_count == 2) {
                            double[] t = readSeveralFloats(inputs[0], max_count)[0];
                            t[1] = readSeveralFloats(inputs[1], max_count)[0][1];
                            return new double[2][] { t, t };
                        }
                        return new double[2][] {
                            readSeveralFloats(inputs[0],max_count)[0],
                            readSeveralFloats(inputs[1],max_count)[0]
                        };
                    } case 'x': {
                        string[] inputs = input.Split(level);
                        double A = readSeveralFloats(inputs[0], max_count)[0][0];
                        double B = readSeveralFloats(inputs[1], max_count)[0][0];
                        return (max_count == 2)
                             ? new double[2][] { new double[] { A, A }, new double[] { B, B } }
                             : new double[2][] { new double[] { A, B }, new double[] { A, B } };
                    } case '\0': {
                        double[][] vals = new double[2][]{new double[2],new double[2]};
                        if (!Double.TryParse(input.Replace('.', ','), out vals[0][0])) {
                            vals[0][0] = vals[1][0] = def;
                        } else {
                            vals[1][0] = vals[0][0];
                        }
                        vals[0][1] = max_count > 2 ? 1.0-def : vals[0][0];
                        vals[1][1] = max_count > 2 ? def : vals[1][0];
                        return vals;
                    }
                }
                return null;
            }

            private static double correctFreqForFORM(double freq, FORM form)
            {
                if (form == FORM.tri)
                    return (freq + freq);
                return freq;
            }

            public static void showHelpScreen()
            {
                Std.Out.WriteLine("\nSynopsys: ToneGenerator [SampleRate *44100*] [Options]");
                Std.Out.WriteLine("\nOptions:  (*'s = default)\n");
                Std.Out.WriteLine("    --[16*|24|32|64]bit               :    datatype of output file.");
                Std.Out.WriteLine("    --[mono|stereo|quadro|5.1*|7.1]   :    output file channel constelation.");
                Std.Out.WriteLine("    --[tonescript=<filename>]         :    parse synthetization parameters from");
                Std.Out.WriteLine("                                           file (.ton) instead of reading stdin.");
                Std.Out.WriteLine("    --log [filename]                  :    will log any input parameters to [filename]");
                Std.Out.WriteLine("                                           (*clue: such logfile will consist from strings");
                Std.Out.WriteLine("                                            of matching syntax accepted as input paramers");
                Std.Out.WriteLine("                                            which means: [filename] later can be given as");
                Std.Out.WriteLine("                                            --tonescript parameter argument. It will");
                Std.Out.WriteLine("                                            generate again identical wave output so.\n\n");
            }

#if SCRIPT
            public static void Main( string[] args )
            {
                logging = false;
                string logfile = "";
                for  (int i = 0; i < args.Length; ++i ) {
                    if( args[i].StartsWith("--log") ) {
                        logfile = args[i].Contains("=")
                                ? args[i].Split('=')[1]
                                :"ToneGenerator.log";
                        logging = true; break;
                    }
                }  if ( logging ) {
                    Std.Init( CreationFlags.TryConsole, logfile );
                } else {
                    Std.Init( CreationFlags.TryConsole );
                }
#else //COMPILE

            public static void generateTone( IAudioOutStream destination, string synthparameters )
            {
                Std.Init( CreationFlags.TryConsole );
                string[] args;
                if( !synthparameters.EndsWith(".ton") ) {
                    synthetizationParameters = synthparameters.Split('\n');
                    synthetizationParameterCount = -1;
                    args = new string[3];
                } else {
                    synthetizationParameters = null;
                    args = new string[4];
                    args[3] = "--tonescript=" + synthparameters;
                } args[0] = destination.GetFormat().SampleRate.ToString();
                args[1] = "--" + destination.GetFrameType().BitDepth.ToString() + "bit";
                args[2] = destination.GetFrameType().ChannelCount == 1 ? "--mono"
                        : destination.GetFrameType().ChannelCount == 2 ? "--stereo"
                        : destination.GetFrameType().ChannelCount == 4 ? "--quadro"
                        : destination.GetFrameType().ChannelCount == 6 ? "--5.1"
                        : destination.GetFrameType().ChannelCount == 8 ? "--7.1"
                        : "";
#endif //
                PcmFormat format = new PcmFormat();
                format.SampleRate = 44100;
                format.BitsPerSample = 16;
                format.NumChannels = 6;
#if DEBUG
                Std.Out.Write( "arguments:" );
#endif
                bool verbose = false;
                for( int i = 0; i < args.Length; ++i )
                {
                    if( (args[i] == "/?") || (args[i] == "-h") || (args[i] == "/h") ) {
                        showHelpScreen(); return;
                    }
                    if( !verbose ) {
                         verbose = args[i] == "-v";
                    }
#if DEBUG
                    Std.Out.WriteLine( " " + i.ToString() + ": " + args[i] );
#endif
                    if( args[i].StartsWith("--") )
                    {
                        string currentArg = args[i].Substring(2);

                        if (currentArg.EndsWith("bit")) {
                            ushort.TryParse( currentArg.Substring(0, 2),
                                             out format.BitsPerSample );
                        } else
                        if (currentArg.StartsWith("tonescript")) {
                            System.IO.FileInfo f = new System.IO.FileInfo(
                                                 currentArg.Split('=')[1] );
                            if( !f.Exists ) {
                                string error = string.Format(
                                    "ERROR: tonescript file '{0}' can't be opened!\n",
                                                   f.FullName );
                                Std.Err.WriteLine( error );
                                showHelpScreen();
                                Std.Err.WriteLine( error );
                                return;
                            } tonscript = f.OpenText();
                        } else
                        switch ( currentArg ) {
                            case "mono":   format.NumChannels = 1; break;
                            case "stereo": format.NumChannels = 2; break;
                            case "quadro": format.NumChannels = 4; break;
                            case "5.1":    format.NumChannels = 6; break;
                            case "7.1":    format.NumChannels = 8; break;
                            case "help":   showHelpScreen(); return;
                        default: break; }
                    } else
                    if( !UInt32.TryParse( args[i], out format.SampleRate ) ) {
                        format.SampleRate = 44100;
                    }
                }

                format.BlockAlign = (ushort)(format.NumChannels * (format.BitsPerSample >> 3));
                format.ByteRate = format.SampleRate * format.BlockAlign;
                format.Tag = format.BitsPerSample >= 32 ? PcmTag.PCMf : PcmTag.PCMs;


                // Synthetization parameters will be requested by stdin...
                // ..or read from .ton file when --tonescript=filename.ton

                Std.Out.WriteLine( "# filename{0}: ", verbose ? " for wave output" : "" );
                string outputFileName = ReadNextParameter();
                if( !outputFileName.EndsWith(".wav") )
                    outputFileName += ".wav";

                uint intput;
                Std.Out.WriteLine( "# duration{0}: ", verbose ? " of wave in ms" : "" );
                while ( !UInt32.TryParse( ReadNextParameter(), out intput ) )
                    Std.Err.WriteLine( "gib zahlen ein du blödmann!" );
                uint frameCount = (uint)( ((double)format.SampleRate / 1000.0) * intput );

                Std.Out.WriteLine("# oscillators: ");
                while( !UInt32.TryParse( ReadNextParameter(), out intput ) )
                    Std.Err.WriteLine( "gib zahlen ein du blödmann!" );

                mixer = new Panorama[intput][];
                for (int i = 0; i < intput; i++) {
                    mixer[i] = new Panorama[2];
                }
                freqs = new double[intput][];
                amps = new double[intput][][];
                osc = new List<Controlled.Float64>[intput];
                lfo = new List<Controlled.Float64>[intput];
#if EXPERIMENTAL
                eq3 = new Controlled.Float64[intput];
                eqGain = new IntPtr[intput][];
                for (int i = 0; i < intput; ++i ) {
                    eq3[i] = new Controlled.Float64(ControlMode.Filter);
                    eq3[i].SetUp(700,7000,format.SampleRate,0,ControlMode.Filter);
                    eqGain[i] = new IntPtr[3];
                }
#endif
                for( int i = 0; i < intput; ++i )
                { // prepare each oscillator and regarding arrays holding frequency and
                  // panorama values which may change over time axis by given parameters....
                  osc[i] = new List<Controlled.Float64>();
                  lfo[i] = new List<Controlled.Float64>();
#if EXPERIMENTAL
                    eq3[i].VAL = 0;
                    unsafe {
                        eqGain[i] = new IntPtr[3];
                        eqGain[i][0] = eq3[i].GetPin(EQ3BandPin.LO_GAIN);
                        eqGain[i][1] = eq3[i].GetPin(EQ3BandPin.MI_GAIN);
                        eqGain[i][2] = eq3[i].GetPin(EQ3BandPin.HI_GAIN);
                        *(double*)eqGain[i][0].ToPointer() = 1.0;
                        *(double*)eqGain[i][1].ToPointer() = 0.0;
                        *(double*)eqGain[i][2].ToPointer() = 0.5;
                        *(bool*)eq3[i].GetPin(EQ3BandPin.BY_PASS).ToPointer() = false;
                    } eq3[i].SetCheckAtGet(false);
                    eq3[i].active = true;
#endif
                    FORM form;
                    string oscNum = (i+1).ToString();
                    Std.Out.WriteLine(
                        string.Format("# osc[{1}] {0}:",verbose?"waveform <sin|saw|pls|tri>":"waveform",oscNum)
                                       );
                    string[] oscarg = new string[1];
                    oscarg[0] = ReadNextParameter();
                    if( oscarg[0].Contains("~") ) {
                        oscarg = oscarg[0].Split('~');
                        form = (FORM)FORM.Parse( typeof(FORM), oscarg[0] );
                        osc[i].Add( new Controlled.Float64((ControlMode)form) );
                        for( int m = 1; m < oscarg.Length; ++m ) {
                            if ( oscarg[m].Contains("=") ) {
                                string[] modArg = oscarg[m].Split('=');
                                oscarg[m] = modArg[1];
                                lfo[i].Add(new Controlled.Float64(ControlMode.PingPong));
                                Std.Out.WriteLine("Added lfo for oscillator {0}",i);
                                if( modArg[0] != "lfo" ) {
                                    form = (FORM)FORM.Parse( typeof(FORM), modArg[0] );
                                    osc[i].Add(new Controlled.Float64((ControlMode)form));
                                }
                            }
                        }
                    } else {
                        form = (FORM)FORM.Parse( typeof(FORM), oscarg[0] );
                        osc[i].Add( new Controlled.Float64((ControlMode)form) );
                    }

                    Std.Out.WriteLine( "# osc[{0}] {1}:", oscNum, verbose ? "position in field of sound <LR[xFR]>[~<LR[xFR]>]":"panorama" );
                    double[][] rp = readSeveralFloats( ReadNextParameter() );
                    mixer[i][0] = new Panorama( (float)rp[0][0], (float)rp[0][1] );
                    mixer[i][1] = new Panorama( (float)rp[1][0], (float)rp[1][1] );
                    mixer[i][1] = Panorama.MovingStep( mixer[i][0], mixer[i][1], frameCount );

                    Std.Out.WriteLine( "# osc[{0}] {1}frequency:",oscNum,verbose?"enter <start[~end]> ":"" );
                    freqs[i] = readSeveralFloats( ReadNextParameter(), 2 )[0];
                    double initialfrequency = freqs[i][0];
                    for ( int c = 0; c < freqs[i].Length; ++c ) {
                        freqs[i][c] = ( 2.0 / ( (double)format.SampleRate / (double)freqs[i][c] ) );
                        //freqs[i][c] = correctFreqForFORM( freqs[i][c], form );
                    }

                    Std.Out.WriteLine( "# osc[{0}] {1}:",oscNum,verbose?"enter either: absolute amplification[~linear change] L1[~L2]: \n# or: ADSR envelop ALxAT[~SLxDT] (*where RT implicitely is T-(AT+DT) where T=1.0 is total length)":"volume" );
                    amps[i] = readSeveralFloats( 1.0, ReadNextParameter() );

                    // setting up the oscillator
                    osc[i][0].SetUp( -1, 1, freqs[i][0], 0, osc[i][0].Mode );
                    osc[i][0].SetCheckAtGet();
                    osc[i][0].Active = true;
                    for(int o=1;o<osc[i].Count;++o) {
                        osc[i][o].SetUp( -1, 1, freqs[i][0], 0, osc[i][o].Mode );
                        osc[i][o].SetCheckAtGet();
                        osc[i][o].LetPoint( ControllerVariable.MOV, osc[i][0].GetPointer(ControllerVariable.MOV) );
                        osc[i][o].Active = true;
                    }

                    // setting up linear frequency change
                    freqs[i][1] = ( freqs[i][1] - osc[i][0].MOV ) / (double)frameCount;

                    // set up LFO (if modulation is enabled) and attach it to the oscillator
                    for ( int l = 0; l < (oscarg.Length-1); ++l ) {
                        float lfoFrq = float.Parse( oscarg[l+1] );
                        double lfoRange = 0;
                        ControlMode mode = osc[i][l].Mode;
                        form = (FORM)mode;
                        Std.Out.WriteLine("osc {0} is set to mode: {1}",i,mode);
                        Std.Out.WriteLine("ControlMode.{0} is: {1}",ControlMode.Sinus,(uint)ControlMode.Sinus);
                        if( l < osc[i].Count ) {
                        switch( mode ) {
                            case ControlMode.Pulse: {
                                Std.Out.WriteLine("Pulse");
                                lfo[i][l].LetPoint( ControllerVariable.VAL, osc[i][l].GetPin( PulsFormer.FORM ) );
                                lfo[i][l].SetUp(-0.75,0.75,correctFreqForFORM(1.5/format.SampleRate/lfoFrq,FORM.tri),0,ControlMode.PingPong);
                            } break;
                            case ControlMode.Stack: {
                                Std.Out.WriteLine("Stack");
                                lfo[i][l].LetPoint( ControllerVariable.VAL, osc[i][l].GetPin( RampFormer.FORM ) );
                                lfo[i][l].SetUp(-1,1,correctFreqForFORM(2.0/format.SampleRate/lfoFrq,FORM.tri),0,ControlMode.PingPong);
                            } break;
                            case ControlMode.Sinus: {
                                Std.Out.WriteLine("Sinus");
                                lfo[i][l].LetPoint( ControllerVariable.VAL, osc[i][l].GetPin( SineFormer.FORM ) );
                                lfo[i][l].SetUp(-1,1,correctFreqForFORM(2.0/format.SampleRate/lfoFrq,FORM.tri),0,ControlMode.PingPong);
                            } break;
                            default: Std.Out.WriteLine("Error: unknown ControlMode of osc {0}",i);
                                     lfo[i] = null;
                                     lfoFrq = 0;
                            break;
                        }
                        } else {
                            // TODO:  make up lfo's for mixing second/further osc's
                            lfo[i][l].SetUp(0,1,correctFreqForFORM(2.0/format.SampleRate/lfoFrq,FORM.tri),0,ControlMode.PingPong);
                        }
                        if( lfoFrq != 0 ) {
                            lfo[i][l].SetCheckAtGet();
                            lfo[i][l].SetReadOnly( true );
                            lfo[i][l].Active = true;
                        }
                    }
                }
#if SCRIPT
                WaveFileWriter destination = new WaveFileWriter();
                destination.Open( outputFileName, ref format );
                Std.Out.WriteLine( destination.GetFrameType().CreateEmptyFrame().GetType().ToString() );
                Std.Out.WriteLine( string.Format( "will write {3} frames {2}channels {0}kHz {1}bit audio",
                                                  format.SampleRate, format.BitsPerSample,
                                                  format.NumChannels, frameCount ) );
#endif
                // do wave generation, by generating tone into 'destination' AudioStream
                generateWaveForm( destination, frameCount, osc, amps, freqs, mixer, lfo );
#if SCRIPT
                destination.Save();
                destination.Close();
#endif
            }

            private static void generateWaveForm( IAudioOutStream audiostream, uint frameCount,
                                                  List<Controlled.Float64>[] oscilators, double[][][] amp,
                                                  double[][] frequencies, Panorama[][] panoramas,
                                                  List<Controlled.Float64>[] modulators )
            {
                // get and store the output stream's frame type for
                // using it as factory instance for creating frames
                // of matching bitdepths and channel counts from it
                AudioFrameType frameFactory = audiostream.GetFrameType();
                uint sampleRate = audiostream.GetFormat().SampleRate;

                IAudioFrame masterFrame  = frameFactory.CreateEmptyFrame();
                IAudioFrame oscmixFrame  = frameFactory.CreateEmptyFrame();
                int OscillatorCount      = oscilators.Length;

                // prepare adsr envelops for each oscillator:
                double[] level = new double[OscillatorCount];
                double[] delta = new double[OscillatorCount];
                double[] timeR = new double[OscillatorCount];
                double[] nextL = new double[OscillatorCount];
                bool[] isBiggerThenTheNextOne = new bool[OscillatorCount];
                int[] index = new int[OscillatorCount];
                for ( int o = 0; o < OscillatorCount; ++o ) {
                    index[o] = 0;
                    level[o] = 0;
                    nextL[o] = amp[o][index[o]][0];
                    isBiggerThenTheNextOne[o] = level[o] > nextL[o];
                    delta[o] = amp[o][index[o]][1];
                    timeR[o] = 1.0-(delta[o]+amp[o][1][1]);
                    delta[o] = ( nextL[o] / ( delta[o] * frameCount ) );
                    if (delta[o] == double.NaN)
                        delta[o] = nextL[o];

                        amp[o][1][1] =(amp[o][1][0] - amp[o][0][0]) / (amp[o][1][1] * frameCount);
                    if( amp[o][1][1] == double.NaN )
                        amp[o][1][1] = amp[o][1][0] - amp[o][0][0];
                }

                int partialChannels = frameFactory.ChannelCount-4;
                partialChannels = partialChannels > 0 ? partialChannels : 0;
                Controlled.Float32[] FreqSplit = new Controlled.Float32[partialChannels];
                IntPtr[][] splitgains = new IntPtr[partialChannels][];

                for ( int c = frameFactory.ChannelCount-1; c >= 4; --c ) unsafe { int C = c-4;
                    if ( c == (frameFactory.ChannelCount-2) ) {
                        // set up frequency gain for the 'Center' speaker
                        FreqSplit[C] = new Controlled.Float32();
                        FreqSplit[C].SetUp( 500, 5800, sampleRate, 0, ControlMode.Filter3Band4Pole );
                        splitgains[C] = new IntPtr[3] {
                            FreqSplit[C].GetPin( (int)EQ3BandFilterPins.loPass.Gain ),
                            FreqSplit[C].GetPin( (int)EQ3BandFilterPins.miBand.Gain ),
                            FreqSplit[C].GetPin( (int)EQ3BandFilterPins.hiPass.Gain )
                        };
#if WIN64
                        *(double*)splitgains[C][0].ToPointer() = 0.0;
                        *(double*)splitgains[C][1].ToPointer() = 1.0;
                        *(double*)splitgains[C][2].ToPointer() = 0.2;
                        *(double*)FreqSplit[C].GetPin( (int)EQ3BandDoubl.InpGain ).ToPointer() = 1.0/3.0;
#else
                        *(float*)splitgains[C][0].ToPointer() = 0.0f;
                        *(float*)splitgains[C][1].ToPointer() = 1.0f;
                        *(float*)splitgains[C][2].ToPointer() = 0.2f;
                        *(float*)FreqSplit[C].GetPin( (int)EQ3BandFilterPins.InpGain ).ToPointer() = 1.0f/3.0f;
#endif
                        } else if ( c == (frameFactory.ChannelCount-1) ) {
                        // set up frequency gain for the .1 (sub-woof) channel
                        FreqSplit[C] = new Controlled.Float32();
                        FreqSplit[C].SetUp( 500, 5800, sampleRate, 0, ControlMode.Filter3Band4Pole );
                        splitgains[C] = new IntPtr[3] {
                            FreqSplit[C].GetPin( (int)EQ3BandFilterPins.loPass.Gain ),
                            FreqSplit[C].GetPin( (int)EQ3BandFilterPins.miBand.Gain ),
                            FreqSplit[C].GetPin( (int)EQ3BandFilterPins.hiPass.Gain )
                        };

#if WIN64
                        *(double*)splitgains[C][0].ToPointer() = 1.0;
                        *(double*)splitgains[C][1].ToPointer() = 0.0;
                        *(double*)splitgains[C][2].ToPointer() = 0.0;
                        *(double*)FreqSplit[C].GetPin( (int)EQ3BandDoubl.InpGain ).ToPointer() = 0.5;
#else
                        *(float*)splitgains[C][0].ToPointer() = 1.0f;
                        *(float*)splitgains[C][1].ToPointer() = 0.0f;
                        *(float*)splitgains[C][2].ToPointer() = 0.0f;
                        *(float*)FreqSplit[C].GetPin( (int)EQ3BandFilterPins.InpGain ).ToPointer() = 0.5f;
#endif
                    } else {
                        // set up frequency gain for upper speakers (on 7.1 and 9.1) and upper rear (on 9.1)
                        FreqSplit[C] = new Controlled.Float32();
                        FreqSplit[C].SetUp( 500, 5800, sampleRate, 0, ControlMode.Filter3Band4Pole );
                        splitgains[C] = new IntPtr[3] {
                            FreqSplit[C].GetPin( (int)EQ3BandFilterPins.loPass.Gain ),
                            FreqSplit[C].GetPin( (int)EQ3BandFilterPins.miBand.Gain ),
                            FreqSplit[C].GetPin( (int)EQ3BandFilterPins.hiPass.Gain )
                        };

#if WIN64
                        *(double*)splitgains[C][0].ToPointer() = 0.0;
                        *(double*)splitgains[C][1].ToPointer() = 0.2;
                        *(double*)splitgains[C][2].ToPointer() = 1.0;
                        *(double*)FreqSplit[C].GetPin( (int)EQ3BandFilterPins.InpGain ).ToPointer() = 0.25;
#else
                        *(float*)splitgains[C][0].ToPointer() = 0.0f;
                        *(float*)splitgains[C][1].ToPointer() = 0.2f;
                        *(float*)splitgains[C][2].ToPointer() = 1.0f;
                        *(float*)FreqSplit[C].GetPin( (int)EQ3BandFilterPins.InpGain ).ToPointer() = 0.25f;
#endif
                    }
                    FreqSplit[C].SetCheckAtSet();
                    FreqSplit[C].Active = true;
                    for( int vorlauf = 0; vorlauf < 3; ++vorlauf )
                        FreqSplit[C].VAL = 0;
                }

                // loop which cycles 'frameCount' times, each time
                // it will write one frame into the output stream
                for ( uint frame = 0; frame < frameCount; ++frame )
                {
                    // silence the 'master' frame for reuse
                    masterFrame.Amp(0);

                    for (int o = 0; o < OscillatorCount; o++)
                    {
                        // silence the 'track' frame for reuse
                        oscmixFrame.Amp(0);

                        // read out mono sample value of oscillator 'o' and amplify by the actual enveloped volume level...
                        double oscillatorsample;
                    //    Std.Out.Write("getting oscillator {0}...",o);
                    //    Std.Out.Write("...consists from {0} partialoscillators...",oscilators[o].Count);

                        if ( oscilators[o].Count > 1 ) {
                            Std.Out.Write("waightened by lfos: {0}",modulators[o].Count);
                            oscillatorsample = modulators[o][0];
                            oscillatorsample = (oscilators[o][0] * (1.0 - oscillatorsample))
                                             + (oscilators[o][1] * oscillatorsample);
                        } else {
                            oscillatorsample = oscilators[o][0];
                        }
                        oscillatorsample *= level[o];
                    //    Std.Out.WriteLine("... level is: {0}",oscillatorsample);

                        // then mix it (converted to target format's sampletype) into the track's current panorama position
                        switch ( frameFactory.BitDepth ) {
                            case 16: oscmixFrame.Mix( (short)(oscillatorsample * 32760) , panoramas[o][0] ); break;
                            case 32: oscmixFrame.Mix( (float)oscillatorsample, panoramas[o][0] ); break;
                            case 64: oscmixFrame.Mix( (double)oscillatorsample, panoramas[o][0] ); break;
                        }

                        // reduce it's level so no clipping may occur when mixed
                        // to master together with the other osc's within the next step..
                        oscmixFrame.Amp( (float)(1.0/OscillatorCount) );

                        // mix the osc 1:1 into the actual master frame, which
                        // so now will contain mix of all oscillators when done...
                        masterFrame.Mix( oscmixFrame, 0.5f );

                        // now, at least prepare any frequency, panorama and volume changes
                        // to be applied to oscillator 'o' BEFORE generating the next frame

                        // 'start-to-end' linear changing frequency and panorama values
                        oscilators[o][0].MOV += frequencies[o][1];
                        panoramas[o][0].addStep( panoramas[o][1] );


                        // ...calculate volume envelop levels for the next frame
                        level[o] += delta[o];
                        if( isBiggerThenTheNextOne[o] ? level[o]<=nextL[o] : level[o] > nextL[o] ) {
                            level[o] = nextL[o];
                            if( index[o]==0 ) {
                                ++index[o];
                                nextL[o] = amp[o][index[o]][0];
                                delta[o] = amp[o][index[o]][1];
                                amp[o][index[o]][1] = -(amp[o][index[o]][0]/(timeR[o]*frameCount));
                                amp[o][index[o]][0] = 0;
                            } else if ( index[o]==1 ) {
                                nextL[o] = amp[o][index[o]][0];
                                delta[o] = amp[o][index[o]][1];
                            } isBiggerThenTheNextOne[o] = level[o] > nextL[o];
                        }

                        // and let the LFO cycle a small step to it's new value for the next frame
                        if( modulators != null )
                            if( modulators[o] != null )
                                for( int mo = 0; mo < modulators[o].Count; ++mo )
                                    modulators[o][mo].Check();
                    }

                    // if format has partial channels, apply their regarding frequency gains to them
                    for ( int c = 4; c < frameFactory.ChannelCount; ++c ) { int s = c-4;
                        switch( frameFactory.BitDepth ) {
                            case 16: {
                                FreqSplit[s].VAL = (float)(short)masterFrame.get_Channel(c)/32760;
                                masterFrame.set_Channel(c,(short)(FreqSplit[s]*32760));
                            break; }
                            case 32: {
                                FreqSplit[s].VAL = (float)masterFrame.get_Channel(c);
                                masterFrame.set_Channel(c,(float)FreqSplit[s]);
                            break; }
                            case 64: {
                                FreqSplit[s].VAL = (float)masterFrame.get_Channel(c);
                                masterFrame.set_Channel(c,(double)FreqSplit[s].VAL);
                            break; }
                        }
                    }
                    // and write the complete master frame into the output stream:
                    audiostream.WriteFrame( masterFrame );
                }
            }
        };
    }
}