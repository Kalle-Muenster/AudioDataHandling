using System;
using System.Collections.Generic;
using System.Text;
using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FileIO;
using Stepflow.Audio.FrameTypes;
using Consola.Test;
using Std = Consola.StdStream;
namespace Core3WaveLibTests
{

    public class TrimmingTests : Test
    {
        public static WaveFileReader loader;
        public WaveFileWriter        writer;
        public Audio                 loaded;
        public List<Type>            typlst;
        public string                testdata;
        public int                   expect;

        public TrimmingTests( List<Type> types, string testdataname, int expectLength )
            : base(true,false)
        {
            typlst = types;
            testdata = testdataname;
            expect = expectLength;
            AddTestCase( "Trimming Tests", bufferTrimmingPerFrameType );
        }

        public static AudioFrameType audioFrameType( Type fromType )
        {
            Consola.StdStream.Out.WriteLine("\nReflecting Constructor: " + fromType.FullName);
            return ( fromType.GetConstructor(new Type[] { typeof(double) }).Invoke(new object[] { 0.0 }) as IAudioFrame ).FrameType;
        }

        private bool PerFrameType( AudioFrameType frameType )
        {
            Std.Out.WriteLine( "Test: Trimming a {0}bit {1}channel buffer",
                               frameType.BitDepth, frameType.ChannelCount );
            loader.Seek(0);
            Audio buffer = loader.Read();
            int bit = frameType.BitDepth;
            int chn = frameType.ChannelCount;
            buffer.convert(bit,chn);
            Std.Out.WriteLine(string.Format("buffer loaded: {0} frames",buffer.FrameCount));
            if ( buffer is AudioBuffer && buffer != null ) {
                (buffer as AudioBuffer).Trim(0.01); }
            Std.Out.WriteLine( "buffer trimmed below threshold {0}: {1} frames", 0.01f, buffer.FrameCount );
            TimeSpan trimmedDuration = buffer.Duration;
            writer.Save(buffer, string.Format("trimmed_{0}.wav", frameType) );  
            if( trimmedDuration.TotalMilliseconds < (expect - 15) || trimmedDuration.TotalMilliseconds > (expect + 15) ) {
                Std.Err.Stream.Put("resulting buffer has a duration of ").Put(trimmedDuration).Put("ms\nFAIL: expected ").Put(TimeSpan.FromMilliseconds(expect)).Put(" ms\n\n").End();
                return false;
            } else {
                Std.Out.Stream.Put("resulting buffer has a duration of ").Put(trimmedDuration).Put("ms (expected ").Put(TimeSpan.FromMilliseconds(expect)).Put("ms) PASS\n\n").End();
                return true;
            }
        }

        private void bufferTrimmingPerFrameType()
        {
            foreach( Type t in typlst ) {
                AudioFrameType type = audioFrameType( t );
                CheckStep( PerFrameType(type), type.ToString() );
            }
        }

        protected override void TestSuite()
        {
            loader = new WaveFileReader("testdata\\" + testdata);
            loaded = loader.ReadAll();
            Std.Out.Stream.Put("TrimmingTests: loaded '").Put(testdata)
                   .Put("' ").Put(loader.Format).Put("of duration: ").Put(loaded.Duration).Put("ms").End();
            writer = new WaveFileWriter(loaded, testdata);
            writer.Flush();
            writer.Close();
        }
    }
}
