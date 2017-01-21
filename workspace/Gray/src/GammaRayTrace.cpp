#include <Gray/GammaRayTrace.h>
#include <Graphics/VisiblePoint.h>
#include <Graphics/ViewableBase.h>
#include <Graphics/ViewableTriangle.h>
#include <Gray/GammaMaterial.h>
#include <Gray/MaterialStack.h>
#include <Physics/Interaction.h>
#include <Physics/Photon.h>

GammaRayTrace::GammaRayTrace()
{
    defaultMat = NULL;

    // Default simulation time is 1 second
    simulationTime = 1.0;

}

GammaRayTrace::~GammaRayTrace()
{
}

void GammaRayTrace::SetFileNameOutput(const std::string & name)
{
    output.SetLogfile(name);
}

void GammaRayTrace::SetDefaultMaterial(GammaMaterial * mat)
{
    defaultMat = mat;
}

void GammaRayTrace::SetSimulationTime(double time)
{
    simulationTime = time;
};

INTER_TYPE GammaRayTrace::GRayTrace(VisiblePoint &visPoint, int TraceDepth, Photon &photon,
                                    MaterialStack& MatStack, long avoidK=-1)
{
    double hitDist;
    GammaMaterial * curMaterial = MatStack.curMaterial();

    if (curMaterial == NULL) {
        output.LogError(photon, ERROR_EMPTY,  0);
//        cout << "ERROR" << endl;
        return ERROR;
    }

    GammaMaterial * nextMaterial;

    if (TraceDepth <= 0) {
        output.LogError(photon, ERROR_TRACE_DEPTH, curMaterial->GetMaterial());
        cout << "ERROR_TRACE_DEPTH" << endl;
        return NO_INTERACTION;
    }

    int intersectNum = SeekIntersectionKd(photon.pos,photon.dir,
                                          &hitDist,visPoint,avoidK );

    if ( intersectNum<0 ) {
        return NO_INTERACTION;
    } else {
        INTER_TYPE inter_type;
        // set detector id in photon
        double prev_energy = photon.energy;
        switch(Interaction::GammaInteraction(photon, hitDist, *curMaterial)) {
            case PHOTOELECTRIC: {
                output.LogPhotoElectric(photon, (*curMaterial));
                return PHOTOELECTRIC;
                break;
            }
            case XRAY_ESCAPE: {
                return XRAY_ESCAPE;
                break;
            }
            case COMPTON: {
                // log interaction to file
                double deposit = prev_energy - photon.energy;
                output.LogCompton(photon, deposit, *curMaterial);
                return GRayTrace(visPoint, TraceDepth - 1, photon,MatStack, avoidK);
                return COMPTON;
                break;
            }
            case NO_INTERACTION: {
                // If not interaction, recursively traverse the in the direction the photon was travelling
                if (visPoint.IsFrontFacing()) {
                    // to enter a detector, we must first go into it, then out
                    switch (visPoint.GetObject().GetViewableType()) {
                    case ViewableBase::Viewable_Triangle:
                        // This detector id will be used to determine if we scatter in a detector
                        // or inside a phantom
                        photon.det_id = ((const ViewableTriangle&)(visPoint.GetObject())).GetDetectorId();
                        break;
                    default:
                        photon.det_id = -1;
                        break;
                    }
                    nextMaterial = dynamic_cast<GammaMaterial*>(&visPoint.GetMaterialMutable());
                    MatStack.PushMaterial(nextMaterial);
                } else if (visPoint.IsBackFacing()) {
                    photon.det_id = -1;
                    MatStack.PopMaterial();
                } else {
                    cout << "ERROR: material has no face\n";
                    exit(1);
                }
                // calculate the time taken to travel distance of the non-interaction
                photon.time += (hitDist * s1_SOL);

                // Make sure not to hit same place in kdtree
                photon.pos = visPoint.GetPosition() + photon.dir * Epsilon;
                inter_type = GRayTrace(visPoint, TraceDepth - 1, photon, MatStack, avoidK);
                switch(inter_type) {
                case PHOTOELECTRIC:
                case COMPTON:
                case ESCAPE_INTERACTION:
                    return inter_type;
                    break;
                default:
                    return NO_INTERACTION;
                    break;
                }
                break;
            }
            default: {
                cout << "ERROR: Interaction not specified\n";
                return NO_INTERACTION;
                break;
            }
        }
    }
}

void GammaRayTrace::GRayTraceSources(void)
{
    const char escape = 0x1B;
    const int num_chars = 70;
    int char_state = 0;
    int tick_mark = 0;

    int num_rays = 0;

    // calculate the number of positrons to throw
    // TODO: need to fix the number of rays because of negative sources
    // FIXME: time should not increase when Inside() of a negative source
    num_rays = sources.GetTotalEvents(simulationTime);

    // create a tick mark so that we fill up 40 chars
    tick_mark = (int)(num_rays / num_chars);
    if (tick_mark == 0) {
        // Make sure we don't have an error later on because of num % 0
        tick_mark = 1;
    }

    // done for PET Benchmark
    //num_rays = 34076000;
    //num_rays = 2938374;

    MaterialStack MatStack;
    MatStack.SetDefault(defaultMat);
    MatStack.PushMaterial(defaultMat);

    VisiblePoint visPoint;
    int TraceDepth;

    //TODO: Make a photon stack, then race trace it
    // text graphics preamble
    cout << "|";

    for (int i = 1; i < num_rays; i++) {
        Source * source = sources.Decay();
        Isotope * isotope = source->GetIsotope();

        output.LogNuclearDecay(((Positron*)isotope)->GetPositron());

        if ((i % 10000)==0) {
            switch(char_state) {
            case 0:
                cout << "\033[1m|\033[0m";
                break;
            case 1:
                cout << "\033[1m/\033[0m";
                break;
            case 2:
                cout << "\033[1m=\033[0m";
                break;
            case 3:
                cout << "\033[1m\\\033[0m";
                break;
            default:
                cout << "\033[1m|\033[0m";
                break;
            }

            char_state++;
            if (char_state == 4) {
                char_state = 0;
            }

            cout << escape;
            cout << "[1D";

            cout.flush();
        }

        // Fun ANSI graphics to do while waiting for simulation
        // this is a simple spinner code
        if ((i % tick_mark) == 0) {
            cout << "\033[32m=\033[0m";
            cout.flush();
        }
        while(!isotope->IsEmpty()) {
            if (isotope == NULL) {
                cerr << "Empty Decay: ERROR\n";
                continue;
            }

            Photon photon = isotope->NextPhoton();

            TraceDepth = 100;
            MatStack.PushMaterial(source->GetMaterial());
            if (GRayTrace(visPoint, TraceDepth, photon, MatStack) == ERROR) {
                cout << "ERROR\n";
            }
            MatStack.ResetMaterial();
        }
    }
    cout << "=|Done.\n";
}

void GammaRayTrace::AddSource(Source & s)
{
    sources.AddSource(s);
}
