using System;
using System.Collections.Generic;
using Consola;
using Stepflow;
using Stepflow.Audio.FrameTypes;
using Stepflow.Audio.FileIO;
using Stepflow.Controller;



namespace Stepflow.Audio.Tests
{
    public class AudioStreamBufferTest : Consola.Tests.TestCase
    {
        private List<AudioFrameType> typlst;
        public AudioStreamBufferTest(bool logall) : base(logall)
        {
             StdStream.Out.WriteLine("\nTesting AudioStreamBuffer functionality on on all suppoterd frame types");
             typlst = WaveFileHandlingTests.Program.frametypes;
        }

        private void RunTest(AudioFrameType typ)
        {
            AudioStreamBuffer bufter = new AudioStreamBuffer( 44100, typ.PcmTypeTag, typ.BitDepth, typ.ChannelCount, 1000, true );
            switch( typ.BitDepth ) {
                case 8: { Controlled.Int8 plup = new Controlled.Int8();
                        plup.SetUp( sbyte.MinValue, sbyte.MaxValue, 1, 0, ControlMode.Sinus );
                        plup.Active = true;
                        for(int i=0; i<1000;++i ) bufter.WriteFrame(plup.VAL);
                        break; }
                case 16: { Controlled.Int16 plup = new Controlled.Int16();
                        plup.SetUp( Int16.MinValue, Int16.MaxValue, 1, 0, ControlMode.Sinus );
                        plup.Active = true;
                        for(int i=0; i<1000;++i ) bufter.WriteFrame(plup.VAL);
                        break; }
                case 24: { Controlled.Int24 plup = new Controlled.Int24();
                        plup.SetUp( Int24.MinValue, Int24.MaxValue, 1, 0, ControlMode.Sinus );
                        plup.Active = true;
                        for(int i=0; i<1000;++i ) bufter.WriteFrame(plup.VAL);
                        break; }
                case 32: { Controlled.Float32 plup = new Controlled.Float32();
                        plup.SetUp( System.Single.MinValue, System.Single.MaxValue, System.Single.Epsilon, 0, ControlMode.Sinus );
                        plup.Active = true;
                        for(int i=0; i<1000;++i ) bufter.WriteFrame(plup.VAL);
                        break; }
                case 64: { Controlled.Float64 plup = new Controlled.Float64();
                        plup.SetUp( System.Double.MinValue, System.Double.MaxValue, System.Double.Epsilon, 0, ControlMode.Sinus );
                        plup.Active = true;
                        for(int i=0; i<1000;++i ) bufter.WriteFrame(plup.VAL);
                        break; }
            } IAudioFrame[] reading = bufter.ReadFrames( bufter.WrittenFrames() );
        }

        protected override void StartRun()
        {
            foreach( AudioFrameType typ in typlst ) {
               RunTest(typ);
            }
        }
    }
}
