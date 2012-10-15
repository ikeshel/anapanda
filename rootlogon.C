{
  gSystem->Load("lib/libAnaPANDA.so");

  TPManager mm;
  
  mm.SetDrawScale(20);

  //  mm.ReadHistoFile("../data/12.09.05_FPGA_LED_PreAmp_Shaper/VPTT_x16_100ns_5kEv.root", 3);
  mm.ReadHistoFile("../data/12.09.05_FPGA_LED_PreAmp_Shaper/VPTT_x16_100ns_10kEv.root", 3);

  // control panel
  //
  TControlBar *bar = new TControlBar("vertical",  "AnaPANDA Control Bar", 1000, 1000);

  bar->AddButton("--Event 100--", "mm.DoEvent(100)",       "event 100");
  bar->AddButton("Start",         ".x macros/start.C",     "Start 100ms");
  bar->AddButton("Stop",          ".x macros/stop.C",      "Stop");
  bar->AddButton("Draw On/Off",   ".x macros/DrawOnOff.C", "Stop");
  bar->AddButton("Quit",          ".q",                    "Quit .q");

  bar->SetButtonWidth(300);

  bar->Show();

}

