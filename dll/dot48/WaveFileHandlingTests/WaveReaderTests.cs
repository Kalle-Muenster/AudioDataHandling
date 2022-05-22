using System;
using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FrameTypes;
using Stepflow.Audio.FileIO;
using Consola;


namespace WaveFileHandlingTests
{
    public class WaveReaderTest
    {
        private int errors;
        private WaveFileWriter writer;
        private WaveFileReader reader;

        public WaveReaderTest()
        {
            errors = 0;
            StdStream.Out.WriteLine("\n################# WaveReader Test ##################");
            StdStream.Out.WriteLine("loading file: .\\testdata\\Input32Wave.wav");
            reader = new WaveFileReader(".\\testdata\\Input32Wave.wav");
            PcmFormat format = reader.Format;
            if( format.BitsPerSample != 32 ) {
                errors++;
                StdStream.Err.WriteLine("format.BitsPerSample NOT is 32 as expected");
            }
            if( format.NumChannels != 2 ) {
                errors++;
                StdStream.Err.WriteLine("format.NumChannnels NOT is 2 as expected");
            }
            if( format.SampleRate != 44100 ) {
                errors++;
                StdStream.Err.WriteLine("format.SampleRate NOT is 44100 as expected");
            }
            StdStream.Out.WriteLine("reader.ReadAll(): .\\testdata\\Input32Wave.wav");
            Audio test = reader.ReadAll();
            writer = new WaveFileWriter();
            writer.Open( "test32_ReadAll.wav", ref format );
            writer.WriteAudio( test );
            writer.Close();
            StdStream.Out.WriteLine("reader.Seek(0)");
            reader.Seek(0);
            StdStream.Out.WriteLine("reader.ReadFrames(100)");
            test = reader.Read(100);
            writer.Save( test, "test32_ReadFrames_100.wav" );
            StdStream.Out.WriteLine("IAudioFrame reader.ReadFrame()");
            IAudioFrame frame = reader.ReadFrame();
            StdStream.Out.WriteLine("IAudioFrame: {0}",frame.ToString());
            StdStream.Out.WriteLine("IAudioFrame Format: {0}",frame.FrameType.CreateFormatStruct(44100).ToString());
            if( ((float)frame.get_Channel(0)==0) && ((float)frame.get_Channel(1)==0) ) {
                StdStream.Err.WriteLine("FAILED: IAudioFrame ReadFrame():  L=0 + R=0");
                errors++;
            } else StdStream.Out.WriteLine("IAudioFrame: L={0},R={1}", frame.get_Channel(0).ToString(), frame.get_Channel(1).ToString() );
            StdStream.Out.WriteLine("ReadFrame() next frame");
            IAudioFrame another = reader.ReadFrame();
            if(((float)frame.get_Channel(0)==(float)another.get_Channel(0)) 
            || ((float)frame.get_Channel(1)==(float)another.get_Channel(1))) {
                 StdStream.Err.WriteLine("FAILED: frame equals next frame... expected different frames");
            }

            reader.Seek(0);
            uint frames = reader.GetLength();
            StdStream.Out.WriteLine("reading whole 32bit file frame wise by ReadFrame()");
            writer = new WaveFileWriter("framewise32bit_ReadFrame.wav",ref format);
            for( uint f = 0; f< frames; ++f ) {
                writer.WriteFrame( reader.ReadFrame() );
            } writer.Close();
            StdStream.Out.WriteLine("stored all read f32_2 frames to file: framewise32bit_ReadFrame.wav");
            reader.Close();


            StdStream.Out.WriteLine("loading file: .\\testdata\\Input24Wave.wav");
            reader.Open(".\\testdata\\Input24Wave.wav");
            format = reader.Format;
            if( format.BitsPerSample != 24 ) {
                errors++;
                StdStream.Err.WriteLine("format.BitsPerSample NOT is 24 as expected");
            }
            if( format.NumChannels != 2 ) {
                errors++;
                StdStream.Err.WriteLine("format.NumChannnels NOT is 2 as expected");
            }
            if( format.SampleRate != 44100 ) {
                errors++;
                StdStream.Err.WriteLine("format.SampleRate NOT is 44100 as expected");
            }
            StdStream.Out.WriteLine("reader.ReadAll(): .\\testdata\\Input24Wave.wav");
            test = reader.ReadAll();
            writer.Save(test,"test24_ReadAll.wav");
           
            StdStream.Out.WriteLine("reader.Seek(0)");
            reader.Seek(0);
            StdStream.Out.WriteLine("reader.ReadFrames(100)");
            test = reader.Read(100);
            writer.Save( test , "test24_ReadFrames_100.wav" );

            StdStream.Out.WriteLine("IAudioFrame reader.ReadFrame()");
            frame = reader.ReadFrame();
            StdStream.Out.WriteLine("IAudioFrame: {0}",frame.ToString());
            StdStream.Out.WriteLine("IAudioFrame Format: {0}",frame.FrameType.CreateFormatStruct(44100).ToString());
            if( ((Int24)frame.get_Channel(0)==Int24.DB0Value) && ((Int24)frame.get_Channel(1)==Int24.DB0Value) ) {
                StdStream.Err.WriteLine("FAILED: IAudioFrame ReadFrame():  L=0 + R=0");
                errors++;
            } else StdStream.Out.WriteLine("IAudioFrame: L={0},R={1}", frame.get_Channel(0).ToString(), frame.get_Channel(1).ToString() );
            StdStream.Out.WriteLine("ReadFrame() next frame");
            another = reader.ReadFrame();
            if(((Int24)frame.get_Channel(0)==(Int24)another.get_Channel(0)) 
            || ((Int24)frame.get_Channel(1)==(Int24)another.get_Channel(1))) {
                 StdStream.Err.WriteLine("FAILED: frame equals next frame... expected different frames");
            }

            reader.Seek(0);
            frames = reader.GetLength();
            StdStream.Out.WriteLine("reading whole 24bit file frame wise by ReadFrame()");
            writer = new WaveFileWriter( "framewise24bit_ReadFrame.wav", ref format );
            for( uint f = 0; f< frames; ++f ) {
                writer.WriteFrame( reader.ReadFrame() );
            } writer.Close();
            StdStream.Out.WriteLine("stored all read s24_2 frames to file: framewise24bit_ReadFrame.wav");
            writer.Close();

            StdStream.Out.WriteLine("loading file: .\\testdata\\Input16Wave.wav");
            reader.Open(".\\testdata\\Input16Wave.wav");
            format = reader.Format;
            if( format.BitsPerSample != 16 ) {
                errors++;
                StdStream.Err.WriteLine("format.BitsPerSample NOT is 16 as expected");
            }
            if( format.NumChannels != 2 ) {
                errors++;
                StdStream.Err.WriteLine("format.NumChannnels NOT is 2 as expected");
            }
            if( format.SampleRate != 44100 ) {
                errors++;
                StdStream.Err.WriteLine("format.SampleRate NOT is 44100 as expected");
            }
            StdStream.Out.WriteLine("reader.ReadAll(): .\\testdata\\Input16Wave.wav");
            test = reader.ReadAll();
            writer.Save( test,"test16_ReadAll.wav");
            (test as AudioBuffer).Dispose();
            StdStream.Out.WriteLine("reader.Seek(0)");
            reader.Seek(0);
            StdStream.Out.WriteLine("reader.ReadFrames(100)");
            test = reader.Read(100);
            writer.Save( test , "test16_ReadFrames_100.wav" );
            (test as AudioBuffer).Dispose();
            StdStream.Out.WriteLine("IAudioFrame reader.ReadFrame()");
            frame = reader.ReadFrame();
            StdStream.Out.WriteLine("IAudioFrame: {0}",frame.ToString());
            StdStream.Out.WriteLine("IAudioFrame Format: {0}",frame.FrameType.CreateFormatStruct(44100).ToString());
            if( ((short)frame.get_Channel(0)==0) && ((short)frame.get_Channel(1)==0) ) {
                StdStream.Err.WriteLine("FAILED: IAudioFrame ReadFrame():  L=0 + R=0");
                errors++;
            } else StdStream.Out.WriteLine("IAudioFrame: L={0},R={1}", frame.get_Channel(0).ToString(), frame.get_Channel(1).ToString() );
            StdStream.Out.WriteLine("ReadFrame() next frame");
            another = reader.ReadFrame();
            if(((short)frame.get_Channel(0)==(short)another.get_Channel(0)) 
            || ((short)frame.get_Channel(1)==(short)another.get_Channel(1))) {
                 StdStream.Err.WriteLine("FAILED: frame equals next frame... expected different frames");
            }

            reader.Seek(0);
            frames = reader.GetLength();
            StdStream.Out.WriteLine("reading whole 16bit file frame wise by ReadFrame()");
            writer = new WaveFileWriter( "framewise16bit_ReadFrame.wav", ref format );
            for( uint f = 0; f< frames; ++f ) {
                writer.WriteFrame( reader.ReadFrame() );
            } writer.Close();
            StdStream.Out.WriteLine("stored all read s16_2 frames to file: framewise16bit_ReadFrame.wav");
            writer.Close();
        }
        public bool wasError()
        {
            return errors > 0;
        }
    }
}