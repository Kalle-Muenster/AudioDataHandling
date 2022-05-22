using System;
using System.Collections.Generic;
using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FileIO;
using Stepflow.Audio.FrameTypes;
using WaveFileHandlingTests;
using Consola;

namespace WaveFileHandlingTests
{
            public class SplittingTest : Consola.Tests.TestCase
            {
                private WaveFileReader reader;
                private WaveFileWriter writer;

                public SplittingTest() : base(Program.verbose)
                {
                    StdStream.Out.WriteLine("\nTesting Audiobuffer splitting functionality on on all suppoterd frame types");
                    reader = new WaveFileReader("./testdata/split_test.wav");
                    writer = new WaveFileWriter("Splittited.wav");
                    

                }

                protected override void StartRun()
                {
                     List<Type> types = Program.GetFrameTypeList();
                     for (int i = 1; i < types.Count; i++)
                     RunTest( ( types[i].GetConstructor( new Type[]{typeof(double)} ).Invoke( new object[]{0.0} ) as IAudioFrame ).FrameType );
                }

                public void RunTest(AudioFrameType frameType)
                {
                    NextStep( string.Format("\nTest: Splitting a {0}bit {1}channel buffer",
                                             frameType.BitDepth, frameType.ChannelCount ) );
                    reader.Seek(0);
                    Audio buffer = reader.Read();
                    int bit = frameType.BitDepth;
                    int chn = frameType.ChannelCount;
                    buffer.convert(bit,chn);
                    StdStream.Out.WriteLine(string.Format("buffer loaded: {0} frames",buffer.FrameCount));
                    Audio[] splitparts = (buffer as AudioBuffer).split(1000,true);
                    int zaehler = 1;
                    foreach( Audio split in splitparts ) {
                        writer.Save(split, string.Format("split_{0}_part{1}.wav", frameType, zaehler++));
                    } StdStream.Out.WriteLine(string.Format("buffer split at 1000 ms"));
                    StepDone(true);
                }
            }
}
