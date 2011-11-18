/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include <ns3/string.h>
#include <fstream>
//#include "ns3/gtk-config-store.h"
using namespace ns3;

int main (int argc, char *argv[])
{	
  CommandLine cmd;
  cmd.Parse (argc, argv);
	
  // to save a template default attribute file run it like this:
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Save --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim
  //
  // to load a previously created default attribute file
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Load --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim

  //ConfigStore inputConfig;
  //inputConfig.ConfigureDefaults ();

  // parse again so you can override default values from the command line
  //cmd.Parse (argc, argv);

  Ptr<LenaHelper> lena = CreateObject<LenaHelper> ();

  //lena->EnableLogComponents ();
  

  LogComponentEnable ("LenaHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("TraceFadingLossModel", LOG_LEVEL_ALL);
  
  lena->SetAttribute ("FadingModel", StringValue ("ns3::TraceFadingLossModel"));
  
  std::ifstream ifTraceFile;
  ifTraceFile.open ("../../src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad", std::ifstream::in);
  if (ifTraceFile.good ())
    {
      // script launched by test.py
      lena->SetFadingModelAttribute ("TraceFilename", StringValue ("../../src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
    }
  else
    {
      // script launched as an example
      lena->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
    }
    
  // these parameters have to setted only in case of the trace format 
  // differs from the standard one, that is
  // - 10 seconds length trace
  // - 10,000 samples
  // - 0.5 seconds for window size
  // - 100 RB
  lena->SetFadingModelAttribute ("TraceLength", TimeValue (Seconds (10.0)));
  lena->SetFadingModelAttribute ("SamplesNum", UintegerValue (10000));
  lena->SetFadingModelAttribute ("WindowSize", TimeValue (Seconds (0.5)));
  lena->SetFadingModelAttribute ("RbNum", UintegerValue (100));
 
  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (1);

  // Install Mobility Model
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::BuildingsMobilityModel");
  mobility.Install (enbNodes);
  mobility.SetMobilityModel ("ns3::BuildingsMobilityModel");
  mobility.Install (ueNodes);

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  //lena->SetSchedulerType ("ns3::RrFfMacScheduler");
  lena->SetSchedulerType ("ns3::PfFfMacScheduler");
  enbDevs = lena->InstallEnbDevice (enbNodes);
  ueDevs = lena->InstallUeDevice (ueNodes);

  // Attach a UE to a eNB
  lena->Attach (ueDevs, enbDevs.Get (0));

  // Activate an EPS bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  lena->ActivateEpsBearer (ueDevs, bearer, LteTft::Default ());


  Simulator::Stop (Seconds (0.005));

  Simulator::Run ();

  //GtkConfigStore config;
  //config.ConfigureAttributes ();

  Simulator::Destroy ();
  return 0;
}
