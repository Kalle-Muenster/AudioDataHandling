using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Std = Consola.StdStream;
using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FrameTypes;
using Stepflow.Audio.FileIO;
using System.Runtime.InteropServices;

namespace WaveFileHandlingTests
{
    public class StructSizes
    {
        private List<string> errors = new List<string>();
        private string current = "";

        public bool wasError()
        {
            bool was = errors.Count > 0;
            if( was ) logErrors();
            return was;
        }

        public void logErrors()
        {
            int count = 0;
            foreach (string error in errors) {
                Std.Err.WriteLine("ERROR({0}): {1}", ++count, error);
            }
        }

        public void logFrameSizeOf<FT>() where FT : IAudioFrame, new()
        {
            try { 
            FT frame = new FT();
            Std.Out.WriteLine("-------------------------------------------------");
            current = String.Format("Au{0}{1}bit{2}ch:",
                                    frame.FrameType.PcmTypeTag,
                                    frame.FrameType.BitDepth,
                                    frame.FrameType.ChannelCount);
            Std.Out.WriteLine( current );
            Std.Out.WriteLine("Logical size by BlockAlign: {0}",
                               frame.FrameType.BlockAlign);
            Std.Out.WriteLine("Concrete size by Marshal.SizeOf<FT>(): {0}",
                                                Marshal.SizeOf<FT>());
            } catch ( Exception err ) {
                errors.Add(string.Format("FrameSizeOf<{0}>(): {1}", current,err.Message) );
            }
        }

        public StructSizes()
        {
            Std.Out.WriteLine("#################################################");
            Std.Out.WriteLine("# TEST: StructSizes                             #");
            LogStructSizes();
            //LogContainerSizes();
            LogFrameSizes();
            Std.Out.WriteLine("# StructSizes Done!                             #");
            Std.Out.WriteLine("#################################################");
        }

        private void LogStructSizes()
        {
            string test = "StructSizes.";
            string state = "";
            try { 
            Std.Out.WriteLine("//////////////////////////////////////////////////");
            Std.Out.WriteLine("// Marshalled Struct Sizes: //////////////////////");
            state = "PcmFormat";
            Std.Out.WriteLine("Size of {1}: {0}", Marshal.SizeOf<PcmFormat>(),state);
            state = "AutioFrameType";
            Std.Out.WriteLine("Size of {1}: {0}", Marshal.SizeOf<AudioFrameType>(),state);
            state = "Panorama";
            Std.Out.WriteLine("Size of {1}: {0}", Marshal.SizeOf<Panorama>(),state);
            state = "WaveHeader";
            Std.Out.WriteLine("Size of {1}: {0}", Marshal.SizeOf<WaveHeader>(),state);
            } catch (Exception err) {
                errors.Add(test + state + ": " + err.Message);
            }
        }

        private void LogFileStream(AbstractWaveFileStream fstream)
        {
            string streamtype = "";
            if( fstream is WaveFileReader )
                streamtype += typeof(WaveFileReader).Name;
            if( fstream is WaveFileWriter )
                streamtype += typeof(WaveFileWriter).Name;
            Std.Out.WriteLine("Size of {0}: {1}",streamtype,Marshal.SizeOf(fstream));
            fstream.Close();
        }

        private void LogContainerSizes()
        {
            string test = "ContainerSizes.";
            string state = "";
            try { 
                Std.Out.WriteLine("//////////////////////////////////////////////////");
                Std.Out.WriteLine("// Marshalled Container Sizes: ///////////////////");
                state = "WaveFileReader";
                LogFileStream(new WaveFileReader());
                state = "WaveFileWriter";
                LogFileStream(new WaveFileWriter());
                state = "AudioBuffer";
                AudioBuffer audioBuffer = new AudioBuffer(AuPCMs16bit2ch.type,44100,1000);
                Std.Out.WriteLine("Size of AudioBuffer(16bitStereo44100,1000frames): {0}", Marshal.SizeOf(audioBuffer));
                state = "CircularBuffer";
                Circular<AuPCMs16bit2ch> circularBuffer = new Circular<AuPCMs16bit2ch>(audioBuffer);
                Std.Out.WriteLine("Size of Circular<AuPCMs16bit2ch>(44100,1000frames): {0}", Marshal.SizeOf<Circular<AuPCMs16bit2ch>>(circularBuffer));
                state = "AudioStreamBuffer";
                AudioStreamBuffer audioStream = new AudioStreamBuffer(circularBuffer);
                Std.Out.WriteLine("Size of AudioStreamBuffer(44100,1000frames): {0}", Marshal.SizeOf(audioStream));
                audioStream.Dispose();
            } catch (Exception err) {
                errors.Add(test + state + ": " + err.Message);
            }
        }

        private void LogFrameSizes()
        {
            logFrameSizeOf<AuPCMs8bit1ch>();
            logFrameSizeOf<AuPCMs8bit2ch>();
            logFrameSizeOf<AuPCMs8bit4ch>();
            logFrameSizeOf<AuPCMs8bit6ch>();
            logFrameSizeOf<AuPCMs8bit8ch>();
            logFrameSizeOf<AuPCMs16bit1ch>();
            logFrameSizeOf<AuPCMs16bit2ch>();
            logFrameSizeOf<AuPCMs16bit4ch>();
            logFrameSizeOf<AuPCMs16bit6ch>();
            logFrameSizeOf<AuPCMs16bit8ch>();
            logFrameSizeOf<AuPCMs24bit1ch>();
            logFrameSizeOf<AuPCMs24bit2ch>();
            logFrameSizeOf<AuPCMs24bit4ch>();
            logFrameSizeOf<AuPCMs24bit6ch>();
            logFrameSizeOf<AuPCMs24bit8ch>();
            logFrameSizeOf<AuPCMf32bit1ch>();
            logFrameSizeOf<AuPCMf32bit2ch>();
            logFrameSizeOf<AuPCMf32bit4ch>();
            logFrameSizeOf<AuPCMf32bit6ch>();
            logFrameSizeOf<AuPCMf32bit8ch>();
            logFrameSizeOf<AuPCMf64bit1ch>();
            logFrameSizeOf<AuPCMf64bit2ch>();
            logFrameSizeOf<AuPCMf64bit4ch>();
            logFrameSizeOf<AuPCMf64bit6ch>();
            logFrameSizeOf<AuPCMf64bit8ch>();
        }
    }
}
