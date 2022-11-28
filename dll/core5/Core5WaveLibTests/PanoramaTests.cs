using System;
using System.Collections.Generic;
using System.Text;
using Consola.Test;
using Stepflow;
using Stepflow.Audio;
using Stepflow.Audio.FrameTypes;


namespace Core3WaveLibTests
{
    public class PanoramaTests : Test
    {
        public PanoramaTests(bool logall,bool logxml)
            : base(logall,logxml)
        {
            AddTestCase( "PanoramaArithmetic", arithmetic );
            AddTestCase( "AssigningAmplifiers", assignment );
        }

        public void arithmetic()
        {
            Consola.StdStream.Out.Write("Testing Panorama mixer type:\n  pan1(0.8,0.5), pan2(0.1,0.2)\n");
            Panorama pan1 = new Panorama(0.8f, 0.5f);
            Panorama pan2 = new Panorama(0.1f, 0.2f);
            CheckStep(pan1.LR == 0.8f && pan1.FR == 0.5f, "pan1 variable has these values it was constructed from {0}", pan1);
            CheckStep(pan2.LR == 0.1f && pan2.FR == 0.2f, "pan2 variable has these values it was constructed from {0}", pan2);
            Panorama pan = pan1 * 2;
            CheckStep(pan.LR == 1.6f && pan.FR == 1.0f, "pan1 * 2 = {0}", pan.ToString() );
            pan = pan2 / 2;
            CheckStep(pan.LR == 0.05f && pan.FR == 0.1f, "pan2 / 2 = {0}", pan.ToString() );
            pan = pan1 + pan2;
            CheckStep(pan.LR == 0.45f && pan.FR == 0.2f, "pan1 + pan2 = {0}", pan.ToString() );
            pan = pan1 - pan2;
            CheckStep(pan.LR == 0.9f && pan.FR == 0.7f, "pan1 - pan2 = {0}", pan.ToString() );
            pan = pan1 * pan2;
            CheckStep(pan.LR == 0.45f && pan.FR == 0.2f, "pan1 * pan2 = {0}", pan.ToString() );
            pan = pan1 / pan2;
            CheckStep(pan.LR == 0.9f && pan.FR == 0.7f, "pan1 / pan2 = {0}", pan.ToString() );
        }

        public void assignment()
        { 
            Panorama pan = Panorama.Neutral;
            Consola.StdStream.Out.WriteLine("Neutral Panorama will have Side values assigned...");
            pan.LeftSpeaker = 0.8f;
            Consola.StdStream.Out.WriteLine("assigned 0.8 to the left speakers:");
            Consola.StdStream.Out.Write(pan.ToPictogram());
            pan.RearSpeaker = 0.1f;
            Consola.StdStream.Out.WriteLine("assigned 0.1 to the rear speakers:");
            Consola.StdStream.Out.Write(pan.ToPictogram());
            pan.RightSpeaker = 0.75f;
            Consola.StdStream.Out.WriteLine("assigned 0.75 to the right speakers:");
            Consola.StdStream.Out.Write(pan.ToPictogram());
            pan.FrontSpeaker = 0.33f;
            Consola.StdStream.Out.WriteLine("assigned 0.33 to the front speakers:");
            Consola.StdStream.Out.Write(pan.ToPictogram());
        }
    }
}
