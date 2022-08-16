double cos_theta_Elas_Ar(double P_pion){
  double m_Ar40 = 37225.0;
  double m_pion = 139.57;

  double P0 = 1000.;
  double E0 = sqrt(P0 * P0 + m_pion * m_pion);

  double denom = 2 * 1000. * P_pion;
  double numer = 2.0 * sqrt(P_pion * P_pion + m_pion * m_pion) * (E0 + m_Ar40) - (2.0 * m_pion * m_pion + 2.0 * m_Ar40 * E0 );

  double cos_theta = numer / denom;
  return cos_theta;
}

double cos_theta_QE(double P_pion, double Eb){
  double m_proton = 938.272;
  double m_neutron = 939.565;
  double m_pion = 139.57;

  double P0 = 1000.;
  double E0 = sqrt(P0 * P0 + m_pion * m_pion);

  double denom = 2 * 1000. * P_pion;
  double numer = 2.0 * sqrt(P_pion * P_pion + m_pion * m_pion) * (E0 + m_proton - Eb) - (2.0 * m_pion * m_pion + (m_proton - Eb) * (m_proton - Eb) - m_proton * m_proton + 2.0 * (m_proton - Eb) * E0 );

  double cos_theta = numer / denom;
  return cos_theta;
}

double cos_theta_Delta_res(double P_pion, double Eb, double syst){
  double m_delta = 1232.;
  double width_delta = 117.;
  m_delta = m_delta + 0.5 * syst * width_delta;
  double m_proton = 938.272;
  double m_neutron = 939.565;
  double m_pion = 139.57;

  double P0 = 1000.;
  double E0 = sqrt(P0 * P0 + m_pion * m_pion);
  
  double denom = 2 * 1000. * P_pion;
  double numer = 2.0 * sqrt(P_pion * P_pion + m_pion * m_pion) * (E0 + m_proton - Eb) - (2.0 * m_pion * m_pion + (m_proton - Eb) * (m_proton - Eb) - m_delta * m_delta + 2.0 * (m_proton - Eb) * E0 );

  double cos_theta = numer / denom;
  return cos_theta;
}


Double_t CauchyDist(Double_t *x, Double_t *par){
  Double_t mean = par[0];
  Double_t fwhm = par[1];
  Double_t height = par[2];

  Double_t arg = x[0]-mean;
  Double_t numer = height * fwhm * fwhm;
  Double_t denom = arg * arg + fwhm * fwhm;

  Double_t func = numer/denom;
  return func;
}

Double_t SB_Dist(Double_t *x, Double_t *par){
  Double_t mean = par[0];
  Double_t height = par[1];
  Double_t sigma = par[2];
  Double_t beta = par[3];
  Double_t a = par[4];
  Double_t b = par[5];

  Double_t arg = x[0]-mean;
  Double_t numer = height;
  Double_t denom = 1. + pow( fabs(arg) / sigma, beta);

  Double_t func = numer / denom + a * x[0] + b;
  return func;
}

void Fit_and_Draw(TString filename, double width){
  TString input_file_dir = getenv("PDStudy_WD");
  TString root_file_path =input_file_dir + "/";

  TFile *f_output = new TFile(root_file_path + "Fit" + filename, "RECREATE");

  TFile *f_mc = new TFile(root_file_path + filename);
  vector<double> Eloss;
  vector<double> Eloss_err;
  vector<double> mean_vec_mc;
  vector<double> std_vec_mc;
  vector<double> mean_vec_data;
  vector<double> std_vec_data;
  for(int i = 0; i < 50; i++){
    TString this_hist_name = Form("delta_EQE_minus_Eb_%d", i);
    TH1D *this_hist = (TH1D*)gDirectory -> Get(this_hist_name);

    int max_bin = this_hist -> GetMaximumBin();
    double max_x = this_hist -> GetBinCenter(max_bin);
    double fit_x_min = max_x - width;
    double fit_x_max = max_x + width;
    TF1 *this_f = new TF1("this_f", SB_Dist, fit_x_min, fit_x_max, 6);
    this_f->SetParameters(max_x, this_hist -> GetMaximum(), 2., 1.);
    this_f->SetParNames("Mean","Height", "Sigma", "Beta", "a", "b");

    this_hist -> Fit(this_f, "R", "", fit_x_min, fit_x_max);
    double this_mean = this_f -> GetParameter(0);
    double this_std = this_f -> GetParError(0);
    mean_vec_mc.push_back(this_mean);
    std_vec_mc.push_back(this_std);

    f_output -> cd();
    this_hist -> SetName(this_hist_name + "_MC");
    this_hist -> Write();
    f_mc -> cd();

    double this_Eloss = i + 0.;
    Eloss.push_back(this_Eloss);
    Eloss_err.push_back(0.5);
  }

  TGraphErrors *Eloss_ratio_mc_gr = new TGraphErrors(50, &Eloss[0], &mean_vec_mc[0], &Eloss_err[0], &std_vec_mc[0]);

  TCanvas *c = new TCanvas("", "", 800, 600);
  gStyle->SetOptStat(0);
  TH1D* template_h = new TH1D("", "", 1., 0., 50.);
  template_h ->GetXaxis() -> SetTitle("E_{Binding} [MeV]");
  template_h ->GetYaxis() -> SetTitle("<E_{beam} - E_{QE,peak}^{-}>");
  template_h -> GetYaxis() -> SetRangeUser(-50, 50);
  template_h -> Draw();

  Eloss_ratio_mc_gr -> SetLineColor(kGreen);
  Eloss_ratio_mc_gr -> SetLineWidth(3);
  Eloss_ratio_mc_gr -> Draw("epsame");

  TLegend *l = new TLegend(0.1, 0.7, 0.4, 0.9);
  l -> AddEntry(Eloss_ratio_mc_gr, "1 GeV MC #pm #sigma", "l");
  l -> Draw("same");

  TLine *this_line = new TLine(0., 0., 50., 0.);
  this_line -> SetLineStyle(5);
  this_line -> SetLineColor(kRed);
  this_line -> Draw("same");

  c -> SaveAs("./output/plots/Eloss_scan_proton_1GeV.pdf");

  c -> Close();
  f_mc -> Close();
}

void Draw_2D(TString filename, TString histname, double x_rebin, double y_rebin){
  TString input_file_dir = getenv("PDStudy_WD");
  TString root_file_path =input_file_dir + "/";

  TFile *f_mc = new TFile(root_file_path + filename);
  TH2D *this_hist = (TH2D *)gDirectory -> Get(histname);
  this_hist -> RebinX(x_rebin);
  this_hist -> RebinY(y_rebin);
  double max_z = this_hist -> GetMaximum();
  TCanvas *c = new TCanvas("", "", 800, 600);
  c -> SetLogz();
  gStyle->SetOptStat(0);
  TH2D* template_h = new TH2D("", "", 1., 0., 1100., 1., 0., 4.);
  template_h -> GetXaxis() -> SetTitle("P_{#pi^{+}}^{Out-going} [MeV]");
  template_h -> GetYaxis() -> SetTitle("#theta_{#pi^{+}}^{Out-going} [rad.]");
  template_h -> GetZaxis() -> SetRangeUser(0.1, max_z * 2.);
  //template_h -> GetZaxis() -> SetRangeUser(1.1, max_z * 1.1);
  template_h -> Draw("colz");

  this_hist -> Draw("colzsame");

  vector<double> P_pion_vec_Elas;
  vector<double> P_pion_vec_QE;
  vector<double> P_pion_vec_delta;
  vector<double> P_pion_vec_delta_up;
  vector<double> P_pion_vec_delta_down;
  vector<double> theta_Elas;
  vector<double> theta_QE;
  vector<double> theta_QE_delta;
  vector<double> theta_QE_delta_up;
  vector<double> theta_QE_delta_down;
  int N_Elas = 0;
  int N_QE = 0;
  int N_QE_delta = 0;
  int N_QE_delta_up = 0;
  int N_QE_delta_down = 0;
  double min_P = 0.;
  double max_P = 1000.;
  double step = 0.1;
  int N_P = (max_P - min_P) / step;
  for(int i = 0; i < N_P + 1; i++){
    double this_P_pion = 0. + step * (i + 0.);
    double this_theta_Elas = acos(cos_theta_Elas_Ar(this_P_pion));
    double this_theta_QE = acos(cos_theta_QE(this_P_pion, 4.));
    double this_theta_QE_delta = acos(cos_theta_Delta_res(this_P_pion, 4., 0.));
    double this_theta_QE_delta_up = acos(cos_theta_Delta_res(this_P_pion, 4., 1.));
    double this_theta_QE_delta_down = acos(cos_theta_Delta_res(this_P_pion, 4., -1.));

    //cout << "this_P_pion : " << this_P_pion << ", this_theta_QE : " <<this_theta_QE << ", this_theta_QE_delta : " << this_theta_QE_delta << endl;
    if(!isnan(this_theta_Elas)){
      P_pion_vec_Elas.push_back(this_P_pion);
      theta_Elas.push_back(this_theta_Elas);
      N_Elas++;
    }
    if(!isnan(this_theta_QE)){
      P_pion_vec_QE.push_back(this_P_pion);
      theta_QE.push_back(this_theta_QE);
      N_QE++;
    }
    if(!isnan(this_theta_QE_delta)){
      P_pion_vec_delta.push_back(this_P_pion);
      theta_QE_delta.push_back(this_theta_QE_delta);
      N_QE_delta++;
    }
    if(!isnan(this_theta_QE_delta_up)){
      P_pion_vec_delta_up.push_back(this_P_pion);
      theta_QE_delta_up.push_back(this_theta_QE_delta_up);
      N_QE_delta_up++;
    }
    if(!isnan(this_theta_QE_delta_down)){
      P_pion_vec_delta_down.push_back(this_P_pion);
      theta_QE_delta_down.push_back(this_theta_QE_delta_down);
      N_QE_delta_down++;
    }
  }

  TGraph *gr_Elas = new TGraph(N_Elas, &P_pion_vec_Elas[0], &theta_Elas[0]);
  TGraph *gr_QE = new TGraph(N_QE, &P_pion_vec_QE[0], &theta_QE[0]);
  TGraph *gr_delta = new TGraph(N_QE_delta, &P_pion_vec_delta[0], &theta_QE_delta[0]);
  TGraph *gr_delta_up = new TGraph(N_QE_delta_up, &P_pion_vec_delta_up[0], &theta_QE_delta_up[0]);
  TGraph *gr_delta_down = new TGraph(N_QE_delta_down, &P_pion_vec_delta_down[0], &theta_QE_delta_down[0]);

  gr_Elas -> SetLineColor(kRed);
  gr_Elas -> SetLineStyle(4);
  gr_QE -> SetLineColor(kRed);
  gr_delta -> SetLineColor(kBlue);
  gr_delta_up -> SetLineColor(kBlue);
  gr_delta_down -> SetLineColor(kBlue);
  gr_delta_up -> SetLineStyle(4);
  gr_delta_down -> SetLineStyle(4);

  gr_Elas -> Draw("lsame");
  gr_QE -> Draw("lsame");
  gr_delta -> Draw("lsame");
  gr_delta_up -> Draw("lsame");
  gr_delta_down -> Draw("lsame");

  TLegend *l = new TLegend(0.5, 0.7, 0.75, 0.85);
  l -> AddEntry(gr_Elas, "Elastic ^{40}Ar", "l");
  l -> AddEntry(gr_QE, "QE_{#pi^{+}}, E_{b} = 4 MeV", "l");
  l -> AddEntry(gr_delta, "QE_{#pi^{+}}^{#Delta^{+} Res.}, E_{b} = 4 MeV", "l");
  l -> AddEntry(gr_delta_up, "m_{#Delta^{+}} #pm #frac{#Gamma}{2}", "l");
  l -> SetFillStyle(4000);
  l -> SetLineColor(kWhite);
  l -> Draw("same");

  gPad->RedrawAxis();

  c -> SaveAs("./output/plots/" + histname + ".pdf");
  c -> Close();
  f_mc -> Close();
}

void Draw_mX(TString filename, TString histname, double x_min, double x_max, double rebin){
  TString input_file_dir = getenv("PDStudy_WD");
  TString root_file_path =input_file_dir + "/";

  TFile *f_mc = new TFile(root_file_path + filename);
  TH1D *this_hist = (TH1D *)gDirectory -> Get(histname);
  this_hist -> Rebin(rebin);
  double max_y = this_hist -> GetMaximum();
  max_y = max_y * 1.5;
  TCanvas *c = new TCanvas("", "", 800, 600);
  //c -> SetLogz();
  gStyle->SetOptStat(0);

  TH1D* template_h = new TH1D("", "", 1., x_min, x_max);
  template_h ->GetXaxis() -> SetTitle("m_{X} [MeV]");
  template_h ->GetYaxis() -> SetTitle("Events");
  template_h -> GetYaxis() -> SetRangeUser(0., max_y);
  template_h -> Draw();  

  this_hist -> Draw("histsame");

  double m_delta = 1232.;
  double width_delta = 117.;
  double m_delta_minus = m_delta - 0.5 * width_delta;
  double m_delta_plus = m_delta + 0.5 * width_delta;
  double m_proton = 938.272;

  TLine *l_proton = new TLine(m_proton, 0., m_proton, max_y);
  l_proton -> SetLineColor(kRed);
  TLine *l_delta = new TLine(m_delta, 0., m_delta, max_y);
  TLine *l_delta_plus = new TLine(m_delta_plus, 0., m_delta_plus, max_y);
  TLine *l_delta_minus = new TLine(m_delta_minus, 0., m_delta_minus, max_y);
  l_delta -> SetLineColor(kBlue);
  l_delta_plus -> SetLineColor(kBlue);
  l_delta_minus -> SetLineColor(kBlue);
  l_delta_plus -> SetLineStyle(4);
  l_delta_minus -> SetLineStyle(4);

  l_proton -> Draw("same");
  l_delta -> Draw("same");
  l_delta_plus -> Draw("same");
  l_delta_minus -> Draw("same");

  c -> SaveAs("./output/plots/" + histname + ".pdf");
  c -> Close();
  f_mc -> Close();

}

void Fitting_Eloss_range(){
  
  //Fit_and_Draw("hists.root", 50.);
  Draw_2D("hists.root", "Outgoing_pion_P_vs_angle", 10., 10.);
  Draw_mX("hists.root", "mX_QE", 0., 1600., 1.);

}
