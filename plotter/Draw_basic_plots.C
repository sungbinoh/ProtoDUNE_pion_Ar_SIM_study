#include "canvas_margin.h"
#include "mylib.h"

void open_file(TString dir, TString histname){

  TString input_file_dir = getenv("PDStudyOutputDir");
  TString version = getenv("PDStudyV");
  TString root_file_path = input_file_dir+ "/" + version + "/PionAnalyzer/" + Simulator + "/PionAnalyzer_Piplus_1GeV_Ar.root";
  TFile *current_file = new TFile ((root_file_path)) ;
  gDirectory->cd(dir);

  TH1D * current_hist = (TH1D*)gDirectory -> Get(histname + "_" + dir);
  if(current_hist){
    current_hist -> SetDirectory(0);
  }
  TH1::AddDirectory(kFALSE);

  maphist[histname + "_" + dir] = current_hist;

  current_file -> Close();
  delete current_file;
}

void draw_signal_Z17bkg(TString dir, TString histname, double xmin, double xmax, bool stack){

  TString title_y = "Events/bin";
  TString nameofhistogram = histname + "_" + dir;
  TString canvas = nameofhistogram;
  TString legend = nameofhistogram;
  TString line = nameofhistogram;
  canvas.Insert(0, "c_");
  legend.Insert(0, "legend_");
  line.Insert(0, "l_");

  mapcanvas[canvas] = new TCanvas(canvas,"",800,800);
  canvas_margin(mapcanvas[canvas]);
  gStyle -> SetOptStat(1111);
  //mapcanvas[canvas] -> SetLogy();

  maplegend[legend] = new TLegend(0.60, 0.80, 0.96, 0.95);

  TH1D *all = (TH1D*)maphist[histname + "_" + dir] -> Clone();
  TH1D *signal = (TH1D*)maphist[histname + "_Signal_" + dir] -> Clone();
  TH1D *Z17 = (TH1D*)maphist[histname + "_17_" +dir] -> Clone();
  
  /*
  all -> GetXaxis() -> SetRangeUser(xmin, xmax);
  signal -> GetXaxis() -> SetRangeUser(xmin, xmax);
  Z17 -> GetXaxis() -> SetRangeUser(xmin, xmax);
  */

  if(stack) Z17 -> Add(signal);

  all -> SetLineColor(kBlack);
  signal -> SetLineColor(kRed);
  Z17 -> SetLineColor(kGreen);

  all -> SetLineWidth(2);
  signal -> SetLineWidth(2);
  Z17 -> SetLineWidth(2);

  TH1D *empty = new TH1D("", "", 1, xmin, xmax);
  empty-> GetXaxis() -> SetTitle(nameofhistogram);  
  empty -> GetYaxis()->SetLabelSize(0.05);;
  empty -> GetYaxis()->SetTitleSize(0.07);;
  empty -> GetYaxis()->SetTitleOffset(1.02);;
  empty -> GetYaxis()->SetTitle("Events/bin");
  empty -> Draw("hist");

  all -> Draw("histsame");
  signal -> Draw("histsame");
  Z17 -> Draw("histsame");

  maplegend[legend] -> AddEntry(all, "All", "l");
  maplegend[legend] -> AddEntry(signal, "Signal", "l");
  if(stack) maplegend[legend] -> AddEntry(Z17, "Z = 17 bkg + Signal", "l");
  else maplegend[legend] -> AddEntry(Z17, "Z = 17 bkg", "l");
  maplegend[legend] -> SetFillColor(kWhite);
  maplegend[legend] -> SetLineColor(kBlack);
  maplegend[legend] -> SetBorderSize(1);
  maplegend[legend] -> SetFillStyle(1001);
  maplegend[legend] -> SetShadowColor(0);
  maplegend[legend] -> SetEntrySeparation(0.3);
  maplegend[legend] -> Draw("same");

  mapcanvas[canvas] -> cd();

  TString pdfname;
  TString WORKING_DIR = getenv("PDStudy_WD");
  pdfname = WORKING_DIR + "/plots/" + Simulator + "/" + histname + "_" + dir + ".pdf";
  mapcanvas[canvas] -> SaveAs(pdfname);
}


void submit_signal_Z17bkg(TString region, TString histname, double xmin, double xmax, bool stack){

  for(int i = 0; i < N_smear_bit; i++){
    // == Run for no smear and All smaer
    if(i == 0 || i == 7){
      TString smear_flag = smear_flags[i];
      TString dir = region + "_" + smear_flag;
      open_file(dir, histname);
      open_file(dir, histname + "_Signal");
      open_file(dir, histname + "_17");
      
      draw_signal_Z17bkg(dir, histname, xmin, xmax, stack);
    }
  }

}

void run_for_smear_bit(TString dir, TString histname){



}

void Draw_basic_plots(int i_simulaor = 0){
  setTDRStyle();

  if(i_simulaor == 0) Simulator = "GEANT4";
  else if(i_simulaor == 1) Simulator = "FLUKA";
  else return;

  submit_signal_Z17bkg("SR1", "M_residual", 36200., 36400., false);

}
