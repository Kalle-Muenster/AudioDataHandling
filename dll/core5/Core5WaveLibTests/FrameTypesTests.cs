using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FileIO;
using Stepflow.Audio.FrameTypes;
using Std = Consola.StdStream;

namespace Core3WaveLibTests
{
    public class FrameTypesTests 
        : Consola.Test.Test
    {
        public IAudioFrame testling;
        public int[] data = new int[8]{1,2,3,10,20,30,40,50};
        public static Assembly asm = null;
        public List<Type> list;

        public static List<Type> reflectTheTypeList() {
            List<Type> list = new List<Type>();
            if( asm==null )
                asm = Assembly.GetAssembly(typeof(IAudioFrame));
            Consola.StdStream.Out.Write( asm.CodeBase );
            int bitnes = 8;
            int channels = 1;
            PcmTag pcmTag = PcmTag.PCMs;

            list = new List<Type>();
            string typenamepattern = "Stepflow.Audio.FrameTypes.Au{0}{1}bit{2}ch";
            Type fT;
            string framename = string.Format(typenamepattern, pcmTag, bitnes, channels);
            while( ( fT = asm.GetType(framename) ) != null ) {
                list.Add(fT);
                if ((channels += 2) == 3)
                    channels--;
                if (channels > 8) {
                    channels = 1;
                    if ( bitnes == 64 )
                        break;
                    if ( bitnes == 32 )
                        bitnes = 64;
                    else {
                        bitnes += 8;
                        if( bitnes == 32 )
                            pcmTag = PcmTag.PCMf;
                    }
                } framename = string.Format(typenamepattern, pcmTag, bitnes, channels);
            } return list;
        }

        public static AudioFrameType audioFrameType( Type fromType )
        {
            Consola.StdStream.Out.WriteLine("\nReflecting Constructor: {0}",fromType.FullName);
            return (fromType.GetConstructor(new Type[] { typeof(double) }).Invoke(new object[] { 0.0 }) as IAudioFrame).FrameType;
        }

        public FrameTypesTests()
            : base(true,false)
        {
            list = reflectTheTypeList();
            AddTestCase( "AudioFrameType Structures", testAllFrameTypes );
        }

        private bool TestFrameType( AudioFrameType frametype )
        {       
            int Steps = 0;
                IAudioFrame testling = frametype.CreateEmptyFrame();
                Consola.StdStream.Out.WriteLine("Testing FrameType: " + frametype.ToString());
                Consola.StdStream.Out.WriteLine("layout: " + testling.GetType().StructLayoutAttribute.Value.ToString() );
                ++Steps;
                PcmTag pcmtag = testling.FrameType.PcmTypeTag;
                int bitnes = testling.FrameType.BitDepth;
                int channels = testling.FrameType.ChannelCount;
                Consola.StdStream.Out.WriteLine(string.Format("bitnes: {0}, channels: {1}", frametype.BitDepth, frametype.ChannelCount));
                try { for (int i = 0; i < testling.FrameType.ChannelCount; i++) {
                        Consola.StdStream.Out.WriteLine(string.Format("    channel {0} before assigning data: {1}", i, testling.get_Channel(i).ToString()));
                        unsafe
                        {
                            switch (bitnes)
                            {
                                case 8: {  sbyte val = (sbyte)data[i]; testling.set_Channel(i,val); break; }
                                case 16: {
                                    if( pcmtag == PcmTag.PCMf ) {
                                        Float16 val = (Float16)data[i];
                                        testling.set_Channel( i, val );
                                    } else {
                                        short val = (short)data[i];
                                        testling.set_Channel( i, val );
                                    } break;
                                }
                                case 24: { Int24 val = (Int24)data[i]; testling.set_Channel(i,val); break; }
                                case 32: { float val = (float)data[i]/127.0f; testling.SetChannel(i, new IntPtr(&val)); break; }
                                case 64: { double val = (double)data[i]/127.0; testling.set_Channel(i,val); break; }
                            }
                        }
                        Consola.StdStream.Out.WriteLine(string.Format("    channel {0} after assigning data: {1}", i, testling.get_Channel(i).ToString()));
                    }
                    ++Steps;
                    Panorama pan = new Panorama(0.8f, 0.5f);
                    Std.Out.Write("will mix mono sample: ");
                    switch (bitnes) {
                        case 8:  { sbyte val  = 33;     Std.Out.Write(val.ToString()); testling.Mix(val, pan); break; }
                        case 16: {
                            if( pcmtag == PcmTag.PCMf ) {
                                Float16 val = Float16.One / (Float16)3.0f;
                                Std.Out.Write(val.ToString()); testling.Mix(val, pan);
                            } else {
                                short val  = 10000; Std.Out.Write(val.ToString()); testling.Mix(val, pan);
                            } break;
                        }
                        case 24: { Int24 val  = Int24.MaxValue/3;  Std.Out.Write(val.ToString()); testling.Mix(val, pan); break; }
                        case 32: { float val  = 0.333f; Std.Out.Write(val.ToString()); testling.Mix(val, pan); break; }
                        case 64: { double val = 0.333;  Std.Out.Write(val.ToString()); testling.Mix(val, pan); break; }
                    } Std.Out.Stream.Put(" into the frames stereo/3d field by Pan(").Put(pan).Put("):")
                             .Put(string.Format("\n  channels nach dem Mix: {0}", testling.get_Channel(0).ToString())).End();
                    for ( int i = 1; i < testling.FrameType.ChannelCount; i++ ) {
                        Consola.StdStream.Out.Write(string.Format(",{0}", testling.get_Channel(i).ToString()));
                    } ++Steps;
                    Std.Out.WriteLine("\n Applying amplification of factor 0.5");
                    testling.Amp(0.5f);
                    Consola.StdStream.Out.Write(string.Format("\n  channels nach Amp(0.5): {0}", testling.get_Channel(0).ToString()));
                    for (int i = 1; i < testling.FrameType.ChannelCount; i++) {
                        Std.Out.Write(string.Format(",{0}", testling.get_Channel(i).ToString()));
                    } if (testling.FrameType.BitDepth >= 32 && testling.FrameType.ChannelCount == 8)
                        Std.Err.WriteLine("\nACHTUNG! !!!FEHLER!!!");
                    ++Steps;
                    Panorama pan2 = new Panorama(0.1f, 0.2f);
                    Panorama pan3 = pan + pan2;
                    Std.Out.WriteLine("\n Applying panorama change of Pan(0.8,0.5)+Pan(0.1,0.2)");
                    testling.Pan(pan3);
                    Consola.StdStream.Out.Stream.Put("\n  channels nach Pan(").Put(pan3).Put("): ").Put(testling.get_Channel(0).ToString()).End();
                    for (int i = 1; i < testling.FrameType.ChannelCount; i++) {
                        Std.Out.Write( string.Format(",{0}", testling.get_Channel(i).ToString()) );
                    } Std.Out.Write("\n");
                    ++Steps;
                } catch (Exception ex) {
                    Std.Out.WriteLine("FEHLER: {0} at FrameTypesTests.Step {1}",ex.ToString(),Steps.ToString());
                    return false;
                } return true;
            }

        protected override void OnStartUp()
        {
            Std.Out.WriteLine("Test: FrameTypesTests");
            Std.Out.WriteLine("TestData is assigned to each channel of frames of each FrameType supported:\n\nTestData array: 1,2,3,10,20,30,40,50\n");
            Std.Out.WriteLine("On each frame operations will be performed: Mix, Amp, Pan.\nAfter each step, resulting new channel values are logget.");
        }

        private void testAllFrameTypes()
        {
            foreach( Type t in list ) {
                AudioFrameType type = audioFrameType( t );
                CheckStep( TestFrameType(type), type.ToString() );
            }
        }
    }
}
