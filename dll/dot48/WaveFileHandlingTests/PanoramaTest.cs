using System;
using System.Collections.Generic;
using Stepflow.Audio;

namespace WaveFileHandlingTests
{
    public class PanoramaTest : Consola.Tests.TestCase
    {
        public static Consola.StdStreams std;
         
        public PanoramaTest() : base(Program.verbose)
        {
            std = Program.std;
        }

        protected override void StartRun()
        {
            NextStep("Panorama");
            bool ok = false;
            try {
            std.Out.WriteLine("assumed two variables: pan1(0.8,0.5), pan2(0.1,0.2)");
            Panorama pan1 = new Panorama(0.8f, 0.5f);
            Panorama pan2 = new Panorama(0.1f, 0.2f);
            Panorama pan = pan1 * 2;
            std.Out.WriteLine("    pan1 * 2 = "+pan.ToString());
            pan = pan2 / 2;
            std.Out.WriteLine("    pan2 / 2 = "+pan.ToString());
            pan = pan1 + pan2;
            std.Out.Stream.Put("    pan1 + pan2 = ").Put(pan.ToString()).Put("\n").End();
            pan = pan1 - pan2;
            std.Out.WriteLine("    pan1 - pan2 = "+ pan.ToString());
            pan = pan1 * pan2;
            std.Out.WriteLine("    pan1 * pan2 = "+ pan.ToString());
            pan = pan1 / pan2;
            std.Out.WriteLine("    pan1 / pan2 = "+ pan.ToString());

            pan = Panorama.Neutral;
            std.Out.WriteLine("Neutral Panorama will have Side values assigned...");
            pan.LeftSpeaker = 0.8f;
            std.Out.WriteLine("assigned 1.5 to the left speakers:");
            std.Out.WriteLine(pan.ToString());
            pan.RearSpeaker = 0.1f;
            std.Out.WriteLine("assigned 0.1 to the rear speakers:");
            std.Out.WriteLine(pan.ToString());
            pan.RightSpeaker = 0.75f;
            std.Out.WriteLine("assigned 0.75 to the right speakers:");
            std.Out.WriteLine(pan.ToString());
            ok = true;
            } catch {
                ok = false;
                setFatal("execution of some implemented function ´has lead to an error", true);
            }
            CountStep(ok, "execution of Panorama implemented member functions");
        }
    }
}
