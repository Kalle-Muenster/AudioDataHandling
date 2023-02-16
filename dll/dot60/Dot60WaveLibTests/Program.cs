using System;
using Stepflow;
using Consola;
using System.IO;
using Stepflow.Audio;
using Stepflow.Audio.FileIO;
using Stepflow.Audio.FrameTypes;

namespace Core3WaveLibTests
{
    class Program
    {
        static int Main(string[] args)
        {
            int failures = 0;
            StdStream.Init( Consola.CreationFlags.UseConsole
                          | Consola.CreationFlags.CreateLog );
            StdStream.Out.WriteLine("WaveLib.dll (Core5) Tests:");
            FrameTypesTests frameTypes = new FrameTypesTests().Run() as FrameTypesTests;
            failures += frameTypes.getFailures();

            Consola.Test.Test trimming = new TrimmingTests(
                frameTypes.list, "trimming216.wav", 1000 ).Run();
            failures += trimming.getFailures();

            Consola.Test.Test panorama = new PanoramaTests(true,false).Run();
            failures += panorama.getFailures();

            StdStream.Out.WriteLine( "Done!" );
            if (failures > 0) {
                StdStream.Err.WriteLine( "{0} FAILURES", failures );
            }

            return failures;            
        }
    }
}
