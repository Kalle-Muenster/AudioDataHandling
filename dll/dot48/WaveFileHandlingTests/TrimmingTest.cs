using System;
using System.Collections.Generic;
using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FileIO;
using Stepflow.Audio.FrameTypes;
using WaveFileHandlingTests;
using Consola;

namespace Stepflow
{
    namespace Audio
    {
        namespace Tests
        {
            public class TrimmingTest : Consola.Tests.TestCase
            {
                private WaveFileReader reader;
                private WaveFileWriter writer;
                private List<AudioFrameType> typlst;

                public TimeSpan expected = TimeSpan.FromMilliseconds(2000);
                public int tollerance = 10;
                public float threshold = 0.01f; 
                public TrimmingTest() : base(Program.verbose)
                {
                    StdStream.Out.WriteLine("\nTesting Audiobuffer trimming functionality on on all suppoterd frame types");
                    reader = new WaveFileReader("./testdata/16bitStereoUntrimmed.wav");
                    writer = new WaveFileWriter("Trimmed.wav");
                    typlst = Program.frametypes;
                }

                private void RunTest( AudioFrameType frameType )
                {
                    NextStep( frameType.ToString() );
                    StdStream.Out.WriteLine( string.Format( "\nTest: Trimming a {0}bit {1}channel buffer",
                                             frameType.BitDepth, frameType.ChannelCount ) );
                    reader.Seek(0);
                    AudioBuffer buffer = reader.Read() as AudioBuffer;
                    int bit = frameType.BitDepth;
                    int chn = frameType.ChannelCount;
                    StdStream.Out.WriteLine(string.Format("buffer loaded: {0}",buffer.convert(frameType).Duration));
                    buffer.Trim( threshold );
                    TimeSpan result = buffer.Duration;
                    TimeSpan difference = result - expected;
                    writer.Save( buffer, string.Format( "trimmed_{0}.wav", frameType ) );
                    bool passed = (difference.TotalMilliseconds > (-tollerance)) && (difference.TotalMilliseconds < tollerance);
                    CountStep( passed,
                        "buffer trimmed by threshold of {0}db \ntrimmed buffer has length: {1} seconds \n(expected was: {2} seconds by tolerate: +/-{3} miliseconds)",
                        threshold, result, expected, tollerance
                    );
                }

                protected override void StartRun()
                {
                    foreach( AudioFrameType typ in typlst ) {
                        RunTest(typ);
                    }
                }
            }
        }
    }
}
