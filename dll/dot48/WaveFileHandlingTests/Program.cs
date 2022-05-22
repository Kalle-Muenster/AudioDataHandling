//css_co /unsafe
/*//css_ref ControlledValues.dll*/
//css_ref WaveFileHandling.dll
using System;
using System.Collections.Generic;
using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FrameTypes;
using Stepflow.Audio.FileIO;
using Stepflow.Audio.Tests;
using System.Runtime.InteropServices;
using System.Reflection;
using Consola;

namespace WaveFileHandlingTests
{

    public class Rasterizer : Controlled.Float32
    {
        private float func(ref float val,ref float min, ref float max, ref float mov)
        {
            if( (max>0) && (val>max) )
                return val = (float)((int)val % (int)max);
            return val = ( mov == 0 ? val
                             : ( (float) ( (int)( val * (int)mov) / (int)mov ) )
                                 );
        }
        public Rasterizer(int raster) : base()
        {
            Mode = ControlMode.Clamp;
            AttachedDelegate = func;
            SetCheckAtSet();
            Active = true;
            MAX = 0;
            MOV = raster;      
        }
        public void SetRaster(int tilescount)
        {
            MOV = (float)tilescount;
        }
        public void SetModulator(int modulator)
        {
            MAX = (float)modulator;
        }
    }




    public class Program
    {
        static Program Instance;
        public static bool verbose = false;
        public static List<AudioFrameType> frametypes;
        public delegate void fertig(string tschuess);
        static event fertig Fertig;
        public static Consola.StdStreams std = new StdStreams( CreationFlags.UseConsole
                                                             | CreationFlags.CreateLog );
        WaveFileReader reader;
        static WaveFileWriter writer;
        Stepflow.Audio.FileIO.WaveHeader header;
        
        public uint CircularBufferTest()
        {
            string namer = "CircularBufferTest";
            namer += "_{0}bit_{1}chn.{2}";
            AudioBuffer audio = reader.ReadAll() as AudioBuffer;
            std.Out.WriteLine("AudioBuffer reader.ReadAll()");
            std.Out.WriteLine("AudioBuffer.FrameType is Au{0}{1}bit{2}ch", audio.Format.Tag,
                               audio.Format.BitsPerSample, audio.Format.NumChannels);
            Circular<AuPCMf32bit2ch> circulum = new Circular<AuPCMf32bit2ch>(2500);
            std.Out.WriteLine("Created Circular<AuPCMf32bit2ch> buffer of 25000 frames length");
            audio.convert(circulum.TypeCode);
            std.Out.WriteLine("AudioBuffer.convert(CircularBuffer.TypeCode)");
            string outputname = string.Format(namer, audio.Format.BitsPerSample, audio.Format.NumChannels, "wav");
            std.Out.WriteLine("AudioBuffer.FrameType is Au{0}{1}bit{2}ch", audio.Format.Tag,
                               audio.Format.BitsPerSample, audio.Format.NumChannels);
            writer.Save( audio, outputname );
            std.Out.WriteLine("writer.Save(AudioBuffer) to file {0}",outputname);

            Audio copyWithOffset = audio.withOffset((int)(audio.FrameCount/2));
            std.Out.WriteLine("Created copy from AudioBuffer.withOffset(AudioBuffer.FrameCount/2))");
            std.Out.WriteLine("Writing 500 frames of copyWithOffset into CircularBuffer");
            circulum.Write( copyWithOffset, 500, 0 );
            std.Out.WriteLine("Then Writing 500 frames framewise circularBuffer.WriteFrame(frame)");
            for(int i=0;i<500;++i ) {
                circulum.WriteFrame(copyWithOffset.GetFrame((uint)(500+i)));
            }
            std.Out.WriteLine("Reading all Audio available from CircularBuffer.FramesAvailable...");
            std.Out.WriteLine("...and pass to WaveFileWriter.Save('firstThousandOfSecondHalf.wav')");
            writer.Save( circulum.Read(circulum.FramesAvailable), "firstThousandOfSecondHalf.wav");
            std.Out.WriteLine("Then write 2000 frames from AudioBuffer to CircularBuffer.write()");
            circulum.Write(audio, 2000, 0);
            std.Out.WriteLine("Again read all Audio available from CircularBuffer.FramesAvailable...");
            std.Out.WriteLine("...and pass to WaveFileWriter.Save('firstTwothousand.wav')");
            writer.Save(circulum.Read(circulum.FramesAvailable), "firstTwothousand.wav");
            return 100;
        }

        public static List<Type> GetFrameTypeList()
        {
            int bitnes = 8;
            int channels = 1;
            Assembly asm = Assembly.GetAssembly(typeof(IAudioFrame));
            std.Out.WriteLine("Reflecting AudioFrame Types from Assembly:");
            std.Out.WriteLine(asm.CodeBase);
            List<Type> types = new List<Type>() { typeof(IAudioFrame) };
            string typenamepattern = "Stepflow.Audio.FrameTypes.AuPCMs{0}bit{1}ch";

            Type fT;
            while ( (fT = asm.GetType( string.Format(typenamepattern,bitnes,channels))) != null ) {
                types.Add(fT);
                if ((channels += 2) == 3)
                    channels--;
                if (channels > 8) {
                    channels = 1;
                    if (bitnes == 64)
                        break;
                    if ((bitnes += 8) > 32)
                        bitnes = 64;
                    if(bitnes == 32) {
                        typenamepattern = "Stepflow.Audio.FrameTypes.AuPCMf{0}bit{1}ch";
                    }
                }
            } return types;
        }

        unsafe public Program(string inputname,string outputname)
        {
            //     TonegeneratorTest();

            Instance = this;
            reader = new WaveFileReader("testdata\\"+inputname);
            std.Out.WriteLine("\nInputfile: "+inputname);
            header = reader.GetHeader();
            writer = new WaveFileWriter();




        }

        public static void TonegeneratorTest()
        {
            //     IAudioOutStream output = new AudioStreamBuffer(44100,16,8,30*44100);
            //     ToneGenerator.generateTone( output, "testtonescript.ton" );
            //     output = new WaveFileWriter((Audio)output, "ScriptGenerated_Buffer.wav");
            IAudioOutStream output = new WaveFileWriter("ScriptGenerated_Buffer.wav",44100,16,2); 
           // (output as WaveFileWriter).Save();
           // (output as WaveFileWriter).Open("ScriptGenerated_Buffer.wav");
            ToneGenerator.generateTone( output, "testtonescript.ton" );
            (output as WaveFileWriter).Flush();
            (output as WaveFileWriter).Close();
        }

        static void Main(string[] args)
        {
            string inpu = "Input16Wave.wav";
            for(int i=0; i < args.Length; ++i ) {
                if( args[i] == "-v" || args[i] == "--verbose" )
                    Program.verbose = true;
                else {
                    inpu = args[i];
                }
            }

            std.Out.WriteLine("TEST_BEGIN");
            std.Err.Log = std.Out.Log;
        
            Program.Fertig += Program_Fertig;
            Instance = new Program(inpu,"OutputWave.wav");

            int failures = (int)Instance.CircularBufferTest();
            bool FAIL = failures > 0;

            if( FAIL = (new StructSizes()).wasError() )
                std.Err.WriteLine("FAILED: StructSizes");

            Consola.Tests.TestCase test = new TestFrameStructs().Run();
            failures += test.getFailures();

            test = new PanoramaTest().Run();
            failures += test.getFailures();

            test = new TrimmingTest().Run();
            failures += test.getFailures();

      //      if( FAIL = (new SplittingTest()).wasError() )
      //          std.Err.WriteLine("FAILED:  SplittingTest");
            
            if( FAIL = (new AppendingTest()).wasError() )
                std.Err.WriteLine("FAILED: AppendingTest");
            if( FAIL = (new WaveReaderTest()).wasError() )
                std.Err.WriteLine("FAILED: WaveReaderTest");

            

            Program_Fertig(FAIL ? "FAIL" : "PASS");
        }

        private static void Program_Fertig( string tschuess )
        {
            Consola.StdStream.Out.WriteLine( tschuess );
        }
    }
}
