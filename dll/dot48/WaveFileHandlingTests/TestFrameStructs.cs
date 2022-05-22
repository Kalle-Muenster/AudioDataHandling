using System;
using System.Reflection;
using System.Collections.Generic;
using System.Threading.Tasks;

using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FrameTypes;
using Stepflow.Audio.FileIO;
using Consola;

namespace WaveFileHandlingTests
{
    public class TestFrameStructs : Consola.Tests.TestCase
    {
        public IAudioFrame testling;
        public int[] data = new int[8]{1,2,3,10,20,30,40,50};
        public StdStreams std = Program.std;

        private int bitnes = 8;
        private int channels = 1;

        public TestFrameStructs() : base(Program.verbose)
        {
            Program.frametypes = new List<AudioFrameType>();
        }

        protected override void StartRun()
        {
            string stepname = "Reflecting List of Audio frame types";
            Assembly asm = Assembly.GetAssembly(typeof(IAudioFrame));
            NextStep(stepname);
            PcmTag pcmtag = PcmTag.PCMs;
            List<Type> types=new List<Type>(){ typeof(IAudioFrame) };
            string typenamepattern = "Stepflow.Audio.FrameTypes.Au{0}{1}bit{2}ch";

            Type fT;
            while( ( fT = asm.GetType(string.Format(typenamepattern, pcmtag, bitnes, channels)) ) != null ) {
                types.Add(fT);
                if ((channels += 2) == 3)
                    channels--;
                if (channels > 8) {
                    channels = 1;
                    if ( bitnes == 64 ) {
                        break;
                    } else if( bitnes == 24 ) {
                        pcmtag = PcmTag.PCMf;
                        bitnes += 8;
                    } else if ( bitnes == 32 ) {
                        bitnes = 64;
                    } else
                        bitnes += 8;
                }
            }
            StepDone( types.Count > 0 );

            foreach( Type frameType in types ) {
                if (frameType.Name == typeof(IAudioFrame).Name)
                    continue;
                PerType( frameType );
            }
        }

        public void PerType(Type frameType)
        {
                NextStep( frameType.FullName );
                std.Out.WriteLine("layout: " + frameType.StructLayoutAttribute.Value.ToString());

                // create an instance of the currently tested frametype by invoking it's reflected constructor
                testling = frameType.GetConstructor(new Type[] {typeof(double)}).Invoke(new object[] {0.0}) as IAudioFrame;
                Program.frametypes.Add( testling.FrameType );

                bitnes = testling.FrameType.BitDepth;
                channels = testling.FrameType.ChannelCount;

                std.Out.WriteLine( string.Format( "bitnes: {0}, channels: {1}",
                                                  bitnes, channels ) );

                try { for (int i = 0; i < testling.FrameType.ChannelCount; i++) {
                        bool result = false;
                        unsafe
                        {
                            switch (bitnes)
                            {
                                case 8: {  sbyte val = (sbyte)data[i]; testling.set_Channel(i,val);
                                           result = val == (sbyte)testling.get_Channel(i); break; }
                                case 16: { short val = (short)data[i]; testling.set_Channel(i,val);
                                           result = val == (short)testling.get_Channel(i); break; }
                                case 24: { Int24 val = (Int24)data[i]; testling.set_Channel(i, val);
                                           result = val == (Int24)testling.get_Channel(i); break; }
                                case 32: { float val = (float)data[i]/127.0f; testling.SetChannel(i, new IntPtr(&val));
                                           result = val == (float)testling.get_Channel(i); break; }
                                case 64: { double val = (double)data[i]/127.0; testling.set_Channel(i,val);
                                           result = val == (double)testling.get_Channel(i); break; }
                            }
                        }
                        CountStep( result, "channel value '{0}' after assigning data {1}",
                                           i.ToString(), testling.get_Channel(i).ToString() );
                    }

                    Panorama pan = new Panorama(0.25f, 0.75f);
                    std.Out.Write("Mixing mono sample of value "); 
                    switch (bitnes)
                    {
                        case 8:  { sbyte val  = 33;     testling.Mix(val, pan); std.Out.Write(val.ToString()); break; }
                        case 16: { short val  = 10000;  testling.Mix(val, pan); std.Out.Write(val.ToString()); break; }
                        case 24: { Int24 val = (Int24)33000; testling.Mix(val, pan); std.Out.Write(val.ToString()); break; }
                        case 32: { float val  = 0.333f; testling.Mix(val, pan); std.Out.Write(val.ToString()); break; }
                        case 64: { double val = 0.333;  testling.Mix(val, pan); std.Out.Write(val.ToString()); break; }
                    }
                    std.Out.WriteLine(" into the frames stereo/3D field panorama positon:");
                    std.Out.WriteLine( pan.ToPictogram() );

                    std.Out.Write(string.Format("\n  channels nach dem Mix: {0}", testling.get_Channel(0).ToString()));
                    for (int i = 1; i < testling.FrameType.ChannelCount; i++) 
                        std.Out.Write(string.Format(",{0}", testling.get_Channel(i).ToString()));
                    std.Out.Write("\n");
                    std.Out.WriteLine( testling.GetPanorama().ToPictogram() );


                    testling.Amp(0.5f);
                    std.Out.Write(string.Format("\n  channels nach amplification (factor 0.5): {0}", testling.get_Channel(0).ToString()));
                    for (int i = 1; i < testling.FrameType.ChannelCount; i++)
                        std.Out.Write(string.Format(",{0}", testling.get_Channel(i).ToString()));

                    if (testling.FrameType.BitDepth >= 32 && testling.FrameType.ChannelCount == 8)
                        std.Err.WriteLine("undefined frametype");
                    std.Out.Write("\n");

                    pan = pan.flipped();
                    std.Out.WriteLine("Will apply Panorama:");
                    std.Out.WriteLine(pan.ToPictogram());
                    std.Out.WriteLine("to actual frame:");
                    std.Out.WriteLine(testling.GetPanorama().ToPictogram());
                    testling.Pan(pan);
                    std.Out.WriteLine("resulting frame panorama now is:");
                    std.Out.WriteLine(testling.GetPanorama().ToPictogram());

                    std.Out.Write(string.Format("\n  channels nach dem pan (0.25,0.75): {0}", testling.get_Channel(0).ToString()));
                    for (int i = 1; i < testling.FrameType.ChannelCount; i++)
                        std.Out.Write(string.Format(",{0}", testling.get_Channel(i).ToString()));

                    std.Out.Write("\n");


                } catch (Exception ex) {
                    std.Err.WriteLine("FEHLER: " + ex.ToString());
                    Program.frametypes.RemoveAt(Program.frametypes.Count - 1);
                }
            }
        };
    }
