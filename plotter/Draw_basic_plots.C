#include "canvas_margin.h"
#include "mylib.h"
#include <sstream>

using namespace std;

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
  mappad[pad1] -> SetBottomMargin( 0.05 );
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
  gStyle->SetOptTitle(0);
  gStyle->SetLineWidth(2);
  double all_max = all -> GetMaximum();
  pad1_template -> SetStats(0);
  pad1_template -> GetXaxis() -> SetTitle(nameofhistogram);  
  pad1_template -> GetXaxis()->SetLabelSize(0);
  pad1_template -> GetXaxis()->SetTitleSize(0);
  pad1_template -> GetYaxis()->SetLabelSize(0.03);
  pad1_template -> GetYaxis()->SetTitleSize(0.07);
  pad1_template -> GetYaxis()->SetTitleOffset(1.02);
  pad1_template -> GetYaxis()->SetTitle("Events");
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
  mappad[pad2] -> SetTopMargin( 0.05 );
  mappad[pad2] -> SetBottomMargin( 0.4 );
  mappad[pad2] -> SetLeftMargin( 0.15 );
  mappad[pad2] -> SetRightMargin( 0.03 );
  mappad[pad2] -> Draw();
  mappad[pad2] -> cd(); 

  TString name_x = "GeV";
  if(histname.Contains("balance")) name_x = "Balance";
  if(histname.Contains("Q_square")) name_x = "GeV^{2}";

  TH1D * pad2_template = new TH1D("", "", 1, xmin, xmax);
  gStyle->SetOptTitle(0);
  gStyle->SetLineWidth(2);
  pad2_template -> Draw("hist");
  pad2_template -> SetTitle("");
  pad2_template -> SetLineColor(kWhite);
  pad2_template -> GetXaxis() -> SetTitle(name_x);
  pad2_template -> GetXaxis() -> SetTitleSize(0.20);
  pad2_template -> GetXaxis() -> SetLabelSize(0.125);
  pad2_template -> GetYaxis() -> SetTitle("Shape #frac{Z17 Bkg.}{Signal}");
  pad2_template -> GetYaxis() -> SetTitleSize(0.10);
  pad2_template -> GetYaxis() -> SetTitleOffset(0.5);
  pad2_template -> GetYaxis() -> SetLabelSize(0.09);
  pad2_template -> GetYaxis() -> SetNdivisions(505);
  pad2_template -> GetYaxis() -> SetRangeUser(0.0, 2.0);
  pad2_template -> SetStats(0);
  pad2_template -> Draw("histsame");

  TH1D *Z17_abs = (TH1D*)maphist[histname + "_17_" +dir] -> Clone();
  TH1D *signal_abs = (TH1D*)maphist[histname + "_Signal_" +dir] -> Clone();
  Z17_abs -> Scale(1./Z17_abs -> Integral());
  signal_abs -> Scale(1./signal_abs -> Integral());
  Z17_abs -> Divide(signal_abs);

  TH1D *error_band = (TH1D*)signal_abs -> Clone();
  int N_bin = error_band -> GetNbinsX();
  cout << "N_bin : " << N_bin << endl;
  for(int i = 1; i < N_bin + 1; i++){
    double current_bin_content = error_band -> GetBinContent(i);
    double current_bin_error = error_band -> GetBinError(i);
    error_band -> SetBinContent(i, 1.);
    error_band -> SetBinError(i, current_bin_error/current_bin_content);
    if(current_bin_content < 1.0e-10) error_band -> SetBinContent(i, 1000.);
  }
  error_band -> SetFillColor(kOrange);
  error_band -> SetLineColor(kWhite);
  error_band -> SetMarkerSize(0);
  error_band -> Draw("E2same");
  Z17_abs -> SetLineColor(kBlack);
  Z17_abs -> SetLineWidth(2);
  Z17_abs -> SetMarkerColor(kBlack);
  Z17_abs -> SetMarkerSize(1.3);
  Z17_abs -> Draw("epsame");

  maplegend["bottom" + legend] = new TLegend(0.2, 0.85, 0.4, 0.95);
  maplegend["bottom" + legend]->SetBorderSize(0);
  maplegend["bottom" + legend]->SetNColumns(3);
  maplegend["bottom" + legend]->AddEntry(error_band, "Signal Stat.", "f");
  //maplegend["bottom" + legend]->AddEntry(data_hist, "Obs./Pred.", "p");
  maplegend["bottom" + legend]->Draw("same");

  TLine *pad2_line = new TLine(xmin, 1, xmax, 1);
  pad2_line -> SetLineStyle(1);
  pad2_line -> SetLineColor(kBlue);
  pad2_line -> Draw("same");
  
  gPad->RedrawAxis();

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

void draw_P_n_N_n(TString dir, TString histname, int ith_n, double xmin, double xmax){

  if(ith_n > 2 || ith_n < 0) return;
  TString ith_string = "";
  if(ith_n == 0) ith_string = "1st";
  if(ith_n == 1) ith_string = "2nd";
  if(ith_n == 2) ith_string = "3rd";

  TString title_y = "Events/bin";
  TString nameofhistogram = histname + "_" + dir;
  TString canvas = nameofhistogram + TString::Itoa(ith_n, 10);
  TString legend = nameofhistogram;
  TString line = nameofhistogram;
  canvas.Insert(0, "c_");
  legend.Insert(0, "legend_");
  line.Insert(0, "l_");

  mapcanvas[canvas] = new TCanvas(canvas,"",800,800);
  canvas_margin(mapcanvas[canvas]);
  gStyle -> SetOptStat(1111);
  mapcanvas[canvas] -> SetLogy();

  maplegend[legend] = new TLegend(0.60, 0.80, 0.90, 0.93);

  double y_max = 0.;
  int N_n_max = 0;
  if(ith_n == 0){
    N_n_max = 3;
    y_max = maphist[histname + "_Nn_1_0th_n_" + dir] -> GetMaximum();
  }
  if(ith_n == 1){
    N_n_max = 2;
    y_max = maphist[histname + "_Nn_2_1th_n_" + dir] -> GetMaximum();
  }
  if(ith_n == 2){
    N_n_max = 1;
    y_max = maphist[histname + "_Nn_3_2th_n_" + dir] -> GetMaximum();
  }

  TH1D *template_h = new TH1D("", "", 1, xmin, xmax);
  gStyle->SetOptTitle(0);
  gStyle->SetLineWidth(2);
  template_h -> SetStats(0);
  template_h -> GetYaxis() -> SetRangeUser(0.1, y_max * 100);
  template_h -> GetYaxis() -> SetLabelSize(0.03);
  template_h -> GetXaxis() -> SetLabelSize(0.02);
  template_h -> GetXaxis() -> SetTitle("P (" + ith_string + " neutron) (GeV)");
  template_h -> GetXaxis() -> SetTitleSize(0.04);
  template_h -> Draw();

  Int_t colors[3] = {632, 416, 600}; // -- 632 : kRed, 416 : kGreen, 600 : kBlue
  for(int i_Nn = 0; i_Nn < 3 - ith_n; i_Nn++){
    int current_Nn = 3 - i_Nn;
    int current_name_int = ith_n * 10 + current_Nn;
    TString current_name = TString::Itoa(current_name_int, 10);
    maphist[histname + current_name] = (TH1D*)maphist[histname + "_Nn_"+ TString::Itoa(current_Nn, 10) + "_" + TString::Itoa(ith_n, 10) + "th_n_" + dir] -> Clone();
    maphist[histname + current_name] -> SetLineWidth(2);
    maphist[histname + current_name] -> SetLineColor(colors[i_Nn]);
    maphist[histname + current_name] -> Draw("same");
    maplegend[legend] -> AddEntry(maphist[histname + current_name], "N_n = " + TString::Itoa(current_Nn, 10), "l");
  }

  maplegend[legend] -> Draw("same");

  TLatex latex_simulator, latex_histname;
  latex_simulator.SetNDC();
  latex_histname.SetNDC();
  latex_histname.SetTextSize(0.035);
  latex_histname.DrawLatex(0.20, 0.96, histname);
  latex_simulator.SetTextSize(0.035);
  latex_simulator.DrawLatex(0.83, 0.96, Simulator);

  TString pdfname;
  TString WORKING_DIR = getenv("PDStudy_WD");
  pdfname = WORKING_DIR + "/plots/" + Simulator + "/" + histname + "_" + ith_string + "_" + dir + ".png";
  mapcanvas[canvas] -> SaveAs(pdfname);
}

void draw_mX_Nn(TString dir, TString histname, double xmin, double xmax){

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

  maplegend[legend] = new TLegend(0.60, 0.80, 0.97, 0.93);

  TH1D *all = (TH1D*)maphist[histname + "_" + dir] -> Clone();
  TH1D *signal = (TH1D*)maphist[histname + "_Signal_" + dir] -> Clone();
  TH1D *Z17_n_1 = (TH1D*)maphist[histname + "_17_Nn_1_" +dir] -> Clone();
  TH1D *Z17_n_2 = (TH1D*)maphist[histname + "_17_Nn_2_" +dir] -> Clone();
  TH1D *Z17_n_3 = (TH1D*)maphist[histname + "_17_Nn_3_" +dir] -> Clone();

  double bin_xmin, bin_xmax;
  bin_xmin = all -> GetXaxis()-> FindBin(xmin);
  bin_xmax = all -> GetXaxis()-> FindBin(xmax) - 1;
  cout << "bin_xmin : " << bin_xmin << ", bin_xmax : " << bin_xmax << endl;
  double N_all = all -> Integral(bin_xmin, bin_xmax);
  double N_signal = signal ->Integral(bin_xmin, bin_xmax);
  double N_Z17_n_1 = Z17_n_1 -> Integral(bin_xmin, bin_xmax);
  double N_Z17_n_2 = Z17_n_2 -> Integral(bin_xmin, bin_xmax);
  double N_Z17_n_3 = Z17_n_3 -> Integral(bin_xmin, bin_xmax);

  all -> GetXaxis() -> SetRangeUser(xmin, xmax);
  signal -> GetXaxis() -> SetRangeUser(xmin, xmax);
  Z17_n_1 -> GetXaxis() -> SetRangeUser(xmin, xmax);
  Z17_n_2 -> GetXaxis() -> SetRangeUser(xmin, xmax);
  Z17_n_3 -> GetXaxis() -> SetRangeUser(xmin, xmax);

  double all_max = all -> GetMaximum();
  TH1D *template_h = new TH1D("", "", 1, xmin, xmax);
  gStyle->SetOptTitle(0);
  gStyle->SetLineWidth(2);
  template_h -> SetStats(0);
  template_h -> GetYaxis() -> SetRangeUser(0.0, all_max * 1.1);
  template_h -> GetYaxis() -> SetLabelSize(0.03);
  template_h -> GetXaxis() -> SetLabelSize(0.02);
  template_h -> GetXaxis() -> SetTitle("mX (GeV)");
  template_h -> GetXaxis() -> SetTitleSize(0.04);
  template_h -> Draw();

  all -> SetLineColor(kBlack);
  signal -> SetLineColor(kRed);
  Z17_n_1 -> SetLineColor(kGreen);
  Z17_n_2 -> SetLineColor(kYellow);
  Z17_n_3 -> SetLineColor(kBlue);

  all -> SetLineWidth(2);
  signal -> SetLineWidth(2);
  Z17_n_1 -> SetLineWidth(2);
  Z17_n_2 -> SetLineWidth(2);
  Z17_n_3 -> SetLineWidth(2);

  Z17_n_1 -> Add(signal);
  Z17_n_2 -> Add(Z17_n_1);
  Z17_n_3 -> Add(Z17_n_2);

  all -> Draw("same");
  signal -> Draw("same");
  Z17_n_1 -> Draw("same");
  Z17_n_2 -> Draw("same");
  Z17_n_3 -> Draw("same");

  ////////////////////////////////////
  // -- Latex
  ////////////////////////////////////
  mapcanvas[canvas] -> cd();
  TLatex latex_simulator, latex_histname;
  latex_simulator.SetNDC();
  latex_histname.SetNDC();
  latex_histname.SetTextSize(0.035);
  latex_histname.DrawLatex(0.20, 0.96, histname);
  latex_simulator.SetTextSize(0.035);
  latex_simulator.DrawLatex(0.83, 0.96, Simulator);

  // -- Add latex for portions
  TString space = ", ";
  //cout << "N_all, N_signal, N_Z17_n_1, N_Z17_n_2, N_Z17_n_3 : " << N_all << space << N_signal << space << N_Z17_n_1 << space << N_Z17_n_2 << space << N_Z17_n_3 << endl;
  double portion_signal = N_signal / N_all * 100.;
  double portion_n_1 = N_Z17_n_1 / N_all * 100.;
  double portion_n_2 = N_Z17_n_2 / N_all * 100.;
  double portion_n_3 = N_Z17_n_3 / N_all * 100.;
  double portion_others = 100. - portion_signal - portion_n_1 - portion_n_2 - portion_n_3;
  stringstream portion_signal_str, portion_n_1_str, portion_n_2_str, portion_n_3_str, portion_others_str;
  portion_signal_str.precision(1);
  portion_n_1_str.precision(1);
  portion_n_2_str.precision(1);
  portion_n_3_str.precision(1);
  portion_others_str.precision(1);
  portion_signal_str << fixed;
  portion_n_1_str << fixed;
  portion_n_2_str << fixed;
  portion_n_3_str << fixed;
  portion_others_str << fixed;
  portion_signal_str << portion_signal;
  portion_n_1_str << portion_n_1;
  portion_n_2_str << portion_n_2;
  portion_n_3_str << portion_n_3;
  portion_others_str << portion_others;
  TString portion_signal_string = portion_signal_str.str();
  TString portion_n_1_string = portion_n_1_str.str();
  TString portion_n_2_string = portion_n_2_str.str();
  TString portion_n_3_string = portion_n_3_str.str();
  TString portion_others_string = portion_others_str.str();
  if(portion_signal < 0.1) portion_signal_string = "< 0.1";
  if(portion_n_1 < 0.1) portion_n_1_string = "< 0.1";
  if(portion_n_2 < 0.1) portion_n_2_string = "< 0.1";
  if(portion_n_3 < 0.1) portion_n_3_string = "< 0.1";
  if(portion_others < 0.1) portion_others_string = "< 0.1";

  TLatex portion_signal_latex, Z17_bkg_latex, portion_n_1_latex, portion_n_2_latex, portion_n_3_latex, portion_others_latex;
  double text_size = 0.042;
  double position_top_y = 0.83;
  double positon_x = 0.2;
  portion_signal_latex.SetNDC();
  portion_n_1_latex.SetNDC();
  portion_n_2_latex.SetNDC();
  portion_n_3_latex.SetNDC();
  Z17_bkg_latex.SetNDC();
  portion_others_latex.SetNDC();
  portion_signal_latex.SetTextSize(text_size);
  portion_n_1_latex.SetTextSize(text_size);
  portion_n_2_latex.SetTextSize(text_size);
  portion_n_3_latex.SetTextSize(text_size);
  Z17_bkg_latex.SetTextSize(text_size);
  portion_others_latex.SetTextSize(text_size);
  portion_signal_latex.SetTextColor(kRed);
  portion_n_1_latex.SetTextColor(kGreen);
  portion_n_2_latex.SetTextColor(kYellow);
  portion_n_3_latex.SetTextColor(kBlue);
  portion_signal_latex.DrawLatex(positon_x, position_top_y, "Signal : " + portion_signal_string + " %");
  Z17_bkg_latex.DrawLatex(positon_x, position_top_y - text_size, "Z = 17 bkgs");
  portion_n_1_latex.DrawLatex(positon_x + 0.03, position_top_y - 2*text_size, "N_n = 1 : " + portion_n_1_string + " %");
  portion_n_2_latex.DrawLatex(positon_x + 0.03, position_top_y - 3*text_size, "N_n = 2 : "+ portion_n_2_string + " %");
  portion_n_3_latex.DrawLatex(positon_x + 0.03, position_top_y - 4*text_size, "N_n = 3 : "+ portion_n_3_string + " %");
  portion_others_latex.DrawLatex(positon_x, position_top_y - 5*text_size, "Others : " + portion_others_string + " %");

  stringstream xmin_str, xmax_str;
  xmin_str.precision(3);
  xmax_str.precision(3);
  xmin_str << fixed;
  xmax_str << fixed;
  xmin_str << xmin;
  xmax_str << xmax;
  TString xmin_string = xmin_str.str();
  TString xmax_string = xmax_str.str();
  TLatex range_latex;
  range_latex.SetNDC();
  range_latex.SetTextSize(0.05);
  range_latex.DrawLatex(0.2, 0.90, "mX = [" + xmin_string + " GeV, " + xmax_string + " GeV]");

  ////////////////////////////////////
  // -- SaveAs
  ////////////////////////////////////
  TString pdfname;
  TString WORKING_DIR = getenv("PDStudy_WD");
  pdfname = WORKING_DIR + "/plots/" + Simulator + "/" + histname + "_Nn_" + dir + +"_" + xmin_string + "_" + xmax_string + ".png";
  mapcanvas[canvas] -> SaveAs(pdfname);

}

void submit_signal_Z17bkg(TString region, TString histname, double xmin, double xmax, bool stack){

  for(int i = 0; i < N_smear_bit; i++){
    // == Run for no smear and All smear
    if(i == 0 || i == 7){
      TString smear_flag = smear_flags[i];
      TString dir = region + "_" + smear_flag;
      open_file(dir, histname);
      open_file(dir, histname + "_Signal");
      open_file(dir, histname + "_17");
      draw_signal_Z17bkg(dir, histname, xmin, xmax, stack);
      
      dir = dir + "_mXcut";
      open_file(dir, histname);
      open_file(dir, histname + "_Signal");
      open_file(dir, histname + "_17");
      draw_signal_Z17bkg(dir, histname, xmin, xmax, stack);
    }
  }

}

void submit_P_n_N_n(TString region, double xmin, double xmax){
  TString histname = "P_neutron";

  for(int i = 0; i < N_smear_bit; i++){
    // == Run for All smear 
    if(i == 7){
      TString smear_flag = smear_flags[i];
      TString dir = region + "_" + smear_flag;
      open_file(dir, histname + "_Nn_1_0th_n");
      open_file(dir, histname + "_Nn_2_0th_n");
      open_file(dir, histname + "_Nn_3_0th_n");
      open_file(dir, histname + "_Nn_2_1th_n");
      open_file(dir, histname + "_Nn_3_1th_n");
      open_file(dir, histname + "_Nn_3_2th_n");
      draw_P_n_N_n(dir, histname, 0, xmin, xmax);
      draw_P_n_N_n(dir, histname, 1, xmin, xmax);
      draw_P_n_N_n(dir, histname, 2, xmin, xmax);

      dir = dir + "_mXcut";
      open_file(dir, histname + "_Nn_1_0th_n");
      open_file(dir, histname + "_Nn_2_0th_n");
      open_file(dir, histname + "_Nn_3_0th_n");
      open_file(dir, histname + "_Nn_2_1th_n");
      open_file(dir, histname + "_Nn_3_1th_n");
      open_file(dir, histname + "_Nn_3_2th_n");
      draw_P_n_N_n(dir, histname, 0, xmin, xmax);
      draw_P_n_N_n(dir, histname, 1, xmin, xmax);
      draw_P_n_N_n(dir, histname, 2, xmin, xmax);
    }
  }
}


void submit_mX_Nn(TString region, TString histname){
  for(int i = 0; i < N_smear_bit; i++){
    // == Run for All smear 
    if(i == 7){
      TString smear_flag = smear_flags[i];
      TString dir = region + "_" + smear_flag;
      open_file(dir, histname);
      open_file(dir, histname + "_Signal");
      open_file(dir, histname + "_17");
      open_file(dir, histname + "_17_Nn_1");
      open_file(dir, histname + "_17_Nn_2");
      open_file(dir, histname + "_17_Nn_3");
      open_file(dir, histname + "_17_Nn_4");
      draw_mX_Nn(dir, histname, 36.2, 36.4);
      draw_mX_Nn(dir, histname, 36.230, 36.310);
      draw_mX_Nn(dir, histname, 36.230, 36.280);
      draw_mX_Nn(dir, histname, 36.230, 36.255);
    }
  }
}

void Draw_basic_plots(int i_simulaor = 0){
  setTDRStyle();

  if(i_simulaor == 0) Simulator = "GEANT4";
  else if(i_simulaor == 1) Simulator = "FLUKA";
  else return;

  // -- bkg vs signal shapes
  /*
  submit_signal_Z17bkg("SR1", "M_residual", 36.200, 36.400, true);
  submit_signal_Z17bkg("SR1", "Q_square", 0., 2., true);
  submit_signal_Z17bkg("SR1", "P_balance_piplus_p", -3., 1., true);
  submit_signal_Z17bkg("SR1", "P_balance_beam_piplus", -0.2, 0.9, true);
  submit_signal_Z17bkg("SR1", "P_balance_beam_p", -0.8, 0.8, true);
  submit_signal_Z17bkg("SR1", "P_proton", 0.2, 1.5, true);
  submit_signal_Z17bkg("SR1", "P_piplus", 0., 1.1, true);
  submit_signal_Z17bkg("SR1", "Delta_E", 0., 1.0, true);
  */
  //submit_signal_Z17bkg("SR1", "P_neutron_Nn_1_0th_n", 0., 1., false);
  
  // -- bkg only shapes
  submit_P_n_N_n("SR1", 0., 1.);

  // -- mX with various N_n
  //submit_mX_Nn("SR1", "M_residual");

}
