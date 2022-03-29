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
  TString pad1 = nameofhistogram;
  TString pad2 = nameofhistogram;
  TString legend = nameofhistogram;
  TString line = nameofhistogram;
  canvas.Insert(0, "c_");
  pad1.Insert(0, "pad1_");
  pad2.Insert(0, "pad2_");
  legend.Insert(0, "legend_");
  line.Insert(0, "l_");

  mapcanvas[canvas] = new TCanvas(canvas,"",800,800);
  canvas_margin(mapcanvas[canvas]);
  gStyle -> SetOptStat(1111);
  //mapcanvas[canvas] -> SetLogy();

  ////////////////////////////////////
  /// == Pad 1
  ////////////////////////////////////
  mappad[pad1] = new TPad("", "", 0, 0.25, 1, 1);
  mappad[pad1] -> SetTopMargin( 0.07 );
  mappad[pad1] -> SetBottomMargin( 0.02 );
  mappad[pad1] -> SetLeftMargin( 0.15 );
  mappad[pad1] -> SetRightMargin( 0.03 );
  mappad[pad1] -> Draw();
  mappad[pad1] -> cd();
  //mappad[pad1] -> SetLogy();

  maplegend[legend] = new TLegend(0.60, 0.80, 0.97, 0.93);

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

  TH1D *pad1_template = new TH1D("", "", 1, xmin, xmax);
  double all_max = all -> GetMaximum();
  pad1_template -> SetStats(0);
  pad1_template -> GetXaxis() -> SetTitle(nameofhistogram);  
  pad1_template -> GetXaxis()->SetLabelSize(0);
  pad1_template -> GetXaxis()->SetTitleSize(0);
  pad1_template -> GetYaxis()->SetLabelSize(0.03);
  pad1_template -> GetYaxis()->SetTitleSize(0.07);
  pad1_template -> GetYaxis()->SetTitleOffset(1.02);
  pad1_template -> GetYaxis()->SetTitle("Events/bin");
  pad1_template -> GetYaxis()->SetRangeUser(0., all_max * 1.3);
  pad1_template -> Draw("hist"); 

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

  ////////////////////////////////////
  /// == Pad 2
  ////////////////////////////////////
  mapcanvas[canvas] -> cd();
  mappad[pad2] = new TPad(pad2, "", 0, 0, 1, 0.25);
  mappad[pad2] -> SetTopMargin( 0.03 );
  mappad[pad2] -> SetBottomMargin( 0.4 );
  mappad[pad2] -> SetLeftMargin( 0.15 );
  mappad[pad2] -> SetRightMargin( 0.03 );
  mappad[pad2] -> Draw();
  mappad[pad2] -> cd();

  TString name_x = "MeV";
  if(histname.Contains("balance")) name_x = "Balance";
  if(histname.Contains("Q_square")) name_x = "GeV^{2}";

  TH1D * pad2_template = new TH1D("", "", 1, xmin, xmax);
  pad2_template -> Draw("hist");
  pad2_template -> SetTitle("");
  pad2_template -> SetLineColor(kWhite);
  pad2_template -> GetXaxis() -> SetTitle(name_x);
  pad2_template -> GetXaxis() -> SetTitleSize(0.15);
  pad2_template -> GetXaxis() -> SetLabelSize(0.10);
  pad2_template -> GetYaxis() -> SetTitle("Shape #frac{Z17 Bkg.}{Signal}");
  pad2_template -> GetYaxis() -> SetTitleSize(0.10);
  pad2_template -> GetYaxis() -> SetTitleOffset(0.5);
  pad2_template -> GetYaxis() -> SetLabelSize(0.08);
  pad2_template -> GetYaxis() -> SetNdivisions(505);
  pad2_template -> GetYaxis() -> SetRangeUser(0.0, 2.5);
  pad2_template -> SetStats(0);
  pad2_template -> Draw("histsame");

  TH1D *Z17_abs = (TH1D*)maphist[histname + "_17_" +dir] -> Clone();
  TH1D *signal_abs = (TH1D*)maphist[histname + "_Signal_" +dir] -> Clone();
  Z17_abs -> Scale(1./Z17_abs -> Integral());
  signal_abs -> Scale(1./signal_abs -> Integral());
  signal_abs -> Divide(Z17_abs);
  signal_abs -> SetLineColor(kBlue);
  signal_abs -> Draw("histsame");

  TLine *pad2_line = new TLine(xmin, 1, xmax, 1);
  pad2_line -> SetLineStyle(1);
  pad2_line -> SetLineColor(kBlue);
  pad2_line -> Draw("same");

  ////////////////////////////////////
  // == Latex
  ////////////////////////////////////
  mapcanvas[canvas] -> cd();
  TLatex latex_simulator, latex_histname;
  latex_simulator.SetNDC();
  latex_histname.SetNDC();
  latex_histname.SetTextSize(0.035);
  latex_histname.DrawLatex(0.20, 0.96, histname);
  latex_simulator.SetTextSize(0.035);
  latex_simulator.DrawLatex(0.83, 0.96, Simulator);

  ////////////////////////////////////
  // == Save as
  ////////////////////////////////////
  TString pdfname;
  TString WORKING_DIR = getenv("PDStudy_WD");
  pdfname = WORKING_DIR + "/plots/" + Simulator + "/" + histname + "_" + dir + ".png";
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

  submit_signal_Z17bkg("SR1", "M_residual", 36200., 36400., true);
  submit_signal_Z17bkg("SR1", "Q_square", 0., 2., true);
  submit_signal_Z17bkg("SR1", "P_balance_piplus_p", -3., 1., true);
  submit_signal_Z17bkg("SR1", "P_balance_beam_piplus", -0.2, 0.9, true);
  submit_signal_Z17bkg("SR1", "P_balance_beam_p", -0.8, 0.8, true);
  submit_signal_Z17bkg("SR1", "P_proton", 200., 1500., true);
  submit_signal_Z17bkg("SR1", "P_piplus", 10., 1100., true);

}
