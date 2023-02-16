using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.AccessControl;
using System.Text;
using System.Threading.Tasks;
using Consola.Test;
using Stepflow.Audio;
using Stepflow.Audio.FileIO;
using Stepflow.Audio.FrameTypes;

namespace Stepflow.Audio.Tests
{
    public class WowbaggerStreamTests : Test
    {
        private string synthese;
        private WaveFileWriter writer;
        public WowbaggerStreamTests(TestResults flags)
            : base(flags) {
            AddTestCase( "Wowbagger der Unendlich verlängerte", perTypeTests );
        }

        private void testPerFrametype( AudioFrameType test )
        {
            int solang = 2000;
            WowbaggerStream testling = new WowbaggerStream(test,44100,1000);
            string filename = string.Format( "Wowbagger_stream_test_{0}", test );
            ToneGenerator.generateTone( testling, filename+string.Format( "\n{0}\n4\n{1}", solang, synthese ) );
            CheckStep( testling.Duration.TotalMilliseconds == solang, "streamed {0} milliseconds of {0} audio", solang, test );
            testling.Compact();
            writer.Save( testling, filename );
            StepInfo("stored resulting buffer to file: {0}.wav", filename);
        }
        private void perTypeTests()
        {
            foreach( AudioFrameType type in WaveFileHandlingTests.Program.frametypes ) 
                if( type.BitDepth > 8 ) testPerFrametype( type );
        }

        protected override void OnStartUp()
        {
            writer = new WaveFileWriter();
            synthese = "sin\n0x1\n2000~200\n0.9x0.4~0.3x0.8\n"
                     + "pls\n1x1\n2000~200\n0.9x0.4~0.3x0.8\n"
                     + "tri\n0x0\n2000~200\n0.9x0.4~0.3x0.8\n"
                     + "saw\n1x0\n2000~200\n0.9x0.4~0.3x0.8\n\n \n";
            base.OnStartUp();
        }
    }
}
