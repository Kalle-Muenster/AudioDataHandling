using System;
using System.Collections.Generic;
using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FileIO;
using Stepflow.Audio.FrameTypes;
using System.Threading.Tasks;
using Consola;

namespace WaveFileHandlingTests
{
    
        
    class AppendingTest
    {
        private int errors;
        public AppendingTest()
        {
            errors = 0;
            StdStream.Out.WriteLine("BEGIN TEST seek, append, compact... loading: trimmed_Stepflow.Audio.FrameTypes.Au32bit6ch.wav");
            WaveFileReader loader = new WaveFileReader("trimmed_Stepflow.Audio.FrameTypes.AuPCMf32bit6ch.wav");
            int halflength = (int)loader.GetLength()/2;
            StdStream.Out.WriteLine("Seek(halfLength) and read 1/2 length to loadeB");
            loader.Seek((uint)halflength);
            Audio loadeB = loader.Read(halflength);
            new WaveFileWriter(loadeB,"loadeB.wav");
            StdStream.Out.WriteLine("Seek(begin) and read 1/2 length to loadeA");
            loader.Seek(0);
            Audio loadeA = loader.Read(halflength);
            new WaveFileWriter(loadeA,"loadeA.wav");
            StdStream.Out.WriteLine("loadeB.append(loadeA).append(loadeA)");
            loadeB.append( loadeA ).append( loadeA );
            if( loadeB.FrameCount != (halflength * 3) ) {
                errors += 1;
                StdStream.Err.WriteLine("loadeB length after appending 2 x loadeA: {0}, expected was: {1}",loadeB.FrameCount,halflength*3);
            } else {
                new WaveFileWriter( loadeB,"appending_Stepflow.Audio.FrameTypes.AuPCMf32bit6ch.wav");
                StdStream.Out.WriteLine("stored both chunks appended as: appending_Stepflow.Audio.FrameTypes.AuPCMf32bit6ch.wav");
            }
        }
        public bool wasError() { return errors > 0; }
    }
}
