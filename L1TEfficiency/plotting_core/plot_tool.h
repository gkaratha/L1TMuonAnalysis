#include "TCanvas.h"
#include "TLegend.h"
#include "TH1.h"
#include "TString.h"
#include <boost/algorithm/string.hpp>


  
  Double_t turnon_func(Double_t *x, Double_t *par){
    double halfpoint = par[0]; double slope = par[1]; double plateau = par[2];
    double offset = 0; double pt = TMath::Max(x[0],0.000001);
    double arg =  (pt - halfpoint)/(TMath::Sqrt(pt)*slope);                         double fitval = offset+0.5*plateau*(1+TMath::Erf(arg));
    return fitval;
 }


void transform(TH1F * heff,std::string kwds){
 //   cout<<"here "<<kwd<<endl;
   std::vector<std::string> cuts;
   boost::split(cuts,kwds,boost::is_any_of("_"));
   for ( std::string cut : cuts){
     std::vector<std::string> value;
     boost::split(value,cut,boost::is_any_of("="));
     TString tstr=value[1];
     for (int ist=0; ist<value.size(); ist++)
       std::cout<<"index "<<ist<<" val "<<value[ist]<<endl;
     if (value[0]=="LineColor") heff->SetLineColor(std::stoi(value[1]));   
     else if (value[0]=="LineWidth") heff->SetLineWidth(std::stoi(value[1]));
     else if (value[0]=="XLabelSize") heff->GetXaxis()->SetTitleSize(std::stof(value[1]));
     else if (value[0]=="YLabelSize") heff->GetYaxis()->SetTitleSize(std::stof(value[1]));
     else if (value[0]=="Title") heff->SetTitle(tstr);
     else if (value[0]=="YTitle") heff->GetYaxis()->SetTitle(tstr);
     else if (value[0]=="Fit"){
       TString function="gauss"; TF1 * f1;
       std::vector<std::string> range;
       boost::split(range,value[1],boost::is_any_of(","));
       if (value[1].find("Gauss")!= std::string::npos)
         f1=new TF1("f1","gauss",std::stof(range[0]),std::stof(range[1]));
       else if (value[1].find("Erf")!= std::string::npos){
         f1=new TF1("f1",turnon_func,std::stof(range[0]),std::stof(range[1]));
         f1->SetParameters(std::stof(range[0]),1.0,0.75);
       }
       else if (value[1].find("Exp")!= std::string::npos)
         f1=new TF1("f1","expo",std::stof(range[0]),std::stof(range[1])); 
       else
         f1=new TF1("f1","pol2",std::stof(range[0]),std::stof(range[1]));
      
       heff->Fit("f1","r");       
     }
    else if (value[0]=="Rate"){
       std::vector<std::string> input;
       boost::split(input,value[1],boost::is_any_of(","));
       TH1F* hclone=(TH1F*) heff->Clone();
       for (int ibin=0; ibin<hclone->GetNbinsX(); ibin++){
          float temp=hclone->Integral(ibin,hclone->GetNbinsX());
          temp=11245*temp*std::stof(input[0])/std::stof(input[1]);
          heff->SetBinContent(ibin,temp);
       }
    }
    else cout<<"uknown option "<<value[0]<<" ommiting "<<endl;
  }
}





TH1F * gethisto(TChain * cc,std::string var,std::string cuts, std::string name, int bins, float start, float end){
  TString name2(name);
 TH1F * htemp=new TH1F(name2,"#font[22]{CMS} #font[12]{Preliminary}",bins,start,end);
 TString var2(var+">>"+name);
 TString cuts2(cuts);
 cc->Draw(var2,cuts2);
 return htemp;
}

void addregion(TChain * cc,TString var,TString cuts, TString name){
 cc->Draw(var+">>+"+name,cuts);
}


TCanvas *canvas_5plot(TH1F * h1, TH1F * h2, TH1F * h3, TH1F * h4, TH1F * h5,TString canvas, bool Logy, bool norm,double miny, double maxy, TString Labelx,TString Labely, TString leg1,TString leg2,TString leg3, TString leg4,TString leg5){

  TCanvas *data_mc_canvas = new TCanvas("DATA_MC_"+canvas,"DATA_MC_"+canvas,700,700);
    gStyle->SetPadBorderMode(0);
  gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);

  h1->SetLineColor(kRed);
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  h2->SetLineColor(kBlue);
  h3->SetLineWidth(2);
  h3->SetLineColor(kBlack);
  h4->SetLineWidth(2);
  h4->SetLineColor(kMagenta);
  h5->SetLineWidth(2);
  h5->SetLineColor(kGreen);

 
  h1->GetYaxis()->SetTitle(Labely);
  h1->GetXaxis()->SetTitle(Labelx);
 // h1->GetYaxis()->SetTitleSize(0.06);
 // h1->GetYaxis()->SetTitleOffset(0.8);
 // h1->GetYaxis()->SetLabelSize(0.05);
 // h1->GetXaxis()->SetTitleSize(0.09);

  if (norm) {
      h1->Scale(1/h1->Integral());
      h2->Scale(1/h2->Integral());
      h3->Scale(1/h3->Integral());
      h4->Scale(1/h4->Integral());
      h5->Scale(1/h5->Integral());
   }

  h1->Draw("HIST ");
  h2->Draw("HIST sames");
  h3->Draw("HIST sames");
  h4->Draw("HIST sames");
  h5->Draw("HIST sames");

  
  h1->GetYaxis()->SetRangeUser(miny,maxy);
  
   TLegend * leg = new TLegend(0.70,0.70,1,1);
   //leg->AddEntry(data_xs,"#font[62]{CMS Preliminary}", "");
   //leg->AddEntry(data_xs,"#font[62]{#font[12]{L}~0.6 fb^{-1}}", "");
   //leg->AddEntry(data_xs,"Run = 304366", "");
   leg->AddEntry(h1,leg1);
   leg->AddEntry(h2,leg2);
   leg->AddEntry(h3,leg3);
   leg->AddEntry(h4,leg4);
   leg->AddEntry(h5,leg5);
   leg->SetTextFont(42);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->SetBorderSize(0);
   leg->Draw();

   if (Logy) data_mc_canvas->SetLogy();

   return data_mc_canvas;

   }



TCanvas *canvas_4plot(TH1F * h1, TH1F * h2, TH1F * h3, TH1F * h4, TString canvas, bool Logy, bool norm,double miny, double maxy, TString Labelx,TString Labely, TString leg1,TString leg2,TString leg3, TString leg4, bool dashed=false){

  TCanvas *data_mc_canvas = new TCanvas("DATA_MC_"+canvas,"DATA_MC_"+canvas,700,700);
    gStyle->SetPadBorderMode(0);
  gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);

  h1->SetLineColor(kRed);
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  h2->SetLineColor(kBlue);
  h3->SetLineWidth(2);
  h3->SetLineColor(kBlack);
  h4->SetLineWidth(2);
  h4->SetLineColor(kMagenta);
if(dashed){
  h1->SetLineColor(kRed);
  h2->SetLineColor(kBlue);
  h3->SetLineColor(kRed);
  h3->SetLineStyle(2);
  h4->SetLineColor(kBlue);
  h4->SetLineStyle(2);
}
 
  h1->GetYaxis()->SetTitle(Labely);
  h1->GetXaxis()->SetTitle(Labelx);
 // h1->GetYaxis()->SetTitleSize(0.06);
 // h1->GetYaxis()->SetTitleOffset(0.8);
 // h1->GetYaxis()->SetLabelSize(0.05);
 // h1->GetXaxis()->SetTitleSize(0.09);

  if (norm) {
      h1->Scale(1/h1->Integral());
      h2->Scale(1/h2->Integral());
      h3->Scale(1/h3->Integral());
      h4->Scale(1/h4->Integral());
   }

  h1->Draw("HIST ");
  h2->Draw("HIST sames");
  h3->Draw("HIST sames");
  h4->Draw("HIST sames");

 if (miny>-1 && maxy >-1)
  h1->GetYaxis()->SetRangeUser(miny,maxy);
  
   TLegend * leg = new TLegend(0.70,0.70,1,1);
   //leg->AddEntry(data_xs,"#font[62]{CMS Preliminary}", "");
   //leg->AddEntry(data_xs,"#font[62]{#font[12]{L}~0.6 fb^{-1}}", "");
   //leg->AddEntry(data_xs,"Run = 304366", "");
   leg->AddEntry(h1,leg1);
   leg->AddEntry(h2,leg2);
   leg->AddEntry(h3,leg3);
   leg->AddEntry(h4,leg4);
   leg->SetTextFont(42);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->SetBorderSize(0);
   leg->Draw();

   if (Logy) data_mc_canvas->SetLogy();

   return data_mc_canvas;

   }

TCanvas *canvas_1plot(TH1F * h1, TString canvas, bool Logy, TString Labelx,TString Labely){

  TCanvas *data_mc_canvas = new TCanvas("DATA_MC_"+canvas,"DATA_MC_"+canvas,700,700);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);
  h1->SetLineWidth(2);
  h1->GetYaxis()->SetTitle(Labely);
  h1->GetXaxis()->SetTitle(Labelx);
  h1->Draw("HIST ");
  if (Logy) data_mc_canvas->SetLogy();
  return data_mc_canvas;
   }

TCanvas *canvas_2plot(TH1F * h1, TH1F * h2, TString canvas, bool Logy, bool norm,double miny, double maxy, TString Labelx,TString Labely, TString leg1,TString leg2,bool dashed=false){

  TCanvas *data_mc_canvas = new TCanvas("DATA_MC_"+canvas,"DATA_MC_"+canvas,700,700);
    gStyle->SetPadBorderMode(0);
  gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);

  h1->SetLineColor(kRed);
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  h2->SetLineColor(kBlue);
 if (dashed){
   h1->SetLineColor(kBlack);
   h2->SetLineColor(kBlack);
   h2->SetLineStyle(2);
  }

 // h1->SetLineStyle(2);
//  h2->SetLineStyle(10);
  h1->GetYaxis()->SetTitle(Labely);
  h1->GetXaxis()->SetTitle(Labelx);
 // h1->GetYaxis()->SetTitleSize(0.06);
 // h1->GetYaxis()->SetTitleOffset(0.8);
 // h1->GetYaxis()->SetLabelSize(0.05);
 // h1->GetXaxis()->SetTitleSize(0.09);

  if (norm) {
      h1->Scale(1/h1->Integral());
      h2->Scale(1/h2->Integral());
   }

  h1->Draw("HIST ");
  h2->Draw("HIST sames");

  if(miny!=-1 && maxy!=-1)
  h1->GetYaxis()->SetRangeUser(miny,maxy);
  
   TLegend * leg = new TLegend(0.70,0.70,1,1);
   //leg->AddEntry(data_xs,"#font[62]{CMS Preliminary}", "");
   //leg->AddEntry(data_xs,"#font[62]{#font[12]{L}~0.6 fb^{-1}}", "");
   //leg->AddEntry(data_xs,"Run = 304366", "");
   leg->AddEntry(h1,leg1);
   leg->AddEntry(h2,leg2);
   leg->SetTextFont(42);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->SetBorderSize(0);
   leg->Draw();

   if (Logy) data_mc_canvas->SetLogy();

   return data_mc_canvas;

   }



TCanvas *canvas_2plot_ratio(TH1F * h1, TH1F * h2, TString canvas, bool Logy, bool norm,double miny, double maxy, TString Labelx,TString Labely, TString leg1,TString leg2){

  TCanvas *data_mc_canvas = new TCanvas("DATA_MC_"+canvas,"DATA_MC_"+canvas,700,700);
    gStyle->SetPadBorderMode(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);
  TPad *pad1 = new TPad("pad1","This is pad1",0.,0.30,1.,1.);
  TPad *pad2 = new TPad("pad2","This is pad2",0.,0.,1.,0.30);
  pad1->SetFillColor(kWhite);
  pad2->SetFillColor(kWhite);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
   gPad->SetBottomMargin(0);
  gPad->SetLeftMargin(0.10); gPad->SetRightMargin(0.03);

  h1->SetLineColor(kRed);
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  h2->SetLineColor(kBlue);

  h1->GetYaxis()->SetTitle(Labely);
  h1->GetXaxis()->SetTitle(Labelx);


  if (norm) {
      h1->Scale(1/h1->Integral());
      h2->Scale(1/h2->Integral());
   }

  h1->Draw("HIST ");
  h2->Draw("HIST sames");

  if(miny!=-1 && maxy!=-1)
  h1->GetYaxis()->SetRangeUser(miny,maxy);
  
   TLegend * leg = new TLegend(0.70,0.70,1,1);
   leg->AddEntry(h1,leg1);
   leg->AddEntry(h2,leg2);
   leg->SetTextFont(42);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->SetBorderSize(0);
   leg->Draw();

   if (Logy) data_mc_canvas->SetLogy();
   pad2->cd();                                                                     gPad->SetTopMargin(0);
   gPad->SetBottomMargin(0.2);
   gPad->SetLeftMargin(0.10); gPad->SetRightMargin(0.03);
   
   TH1F* hdif1=(TH1F*)h1->Clone();
    TH1F* hdif2=(TH1F*)h2->Clone();
     hdif1->SetTitle(" ");
    hdif1->GetYaxis()->SetTitle("Ratio ("+leg1+"/"+leg2+")");
    hdif1->GetYaxis()->SetLabelSize(0.07);
    hdif1->GetYaxis()->SetTitleSize(0.1);
    hdif1->GetYaxis()->SetTitleOffset(0.4);
   hdif1->GetXaxis()->SetLabelSize(0.1);
    hdif1->GetXaxis()->SetTitleSize(0.1);
    hdif1->GetXaxis()->SetTitleOffset(0.7);

    hdif1->Divide(hdif2);
    hdif1->Draw("P");
    hdif1->SetMarkerStyle(kFullCircle);
   hdif1->SetMarkerSize(1);
   return data_mc_canvas;

   }

TCanvas *canvas_2plot_dif(TH1F * h1, TH1F * h2, TString canvas, bool Logy, bool norm,double miny, double maxy, TString Labelx,TString Labely, TString leg1,TString leg2){

  TCanvas *data_mc_canvas = new TCanvas("DATA_MC_"+canvas,"DATA_MC_"+canvas,700,700);
    gStyle->SetPadBorderMode(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);
  TPad *pad1 = new TPad("pad1","This is pad1",0.,0.30,1.,1.);
  TPad *pad2 = new TPad("pad2","This is pad2",0.,0.,1.,0.30);
  pad1->SetFillColor(kWhite);
  pad2->SetFillColor(kWhite);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
   gPad->SetBottomMargin(0);
  gPad->SetLeftMargin(0.10); gPad->SetRightMargin(0.03);

  h1->SetLineColor(kRed);
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  h2->SetLineColor(kBlue);

  h1->GetYaxis()->SetTitle(Labely);
  h1->GetXaxis()->SetTitle(Labelx);


  if (norm) {
      h1->Scale(1/h1->Integral());
      h2->Scale(1/h2->Integral());
   }

  h1->Draw("HIST ");
  h2->Draw("HIST sames");

  if(miny!=-1 && maxy!=-1)
  h1->GetYaxis()->SetRangeUser(miny,maxy);
  
   TLegend * leg = new TLegend(0.70,0.70,1,1);
   leg->AddEntry(h1,leg1);
   leg->AddEntry(h2,leg2);
   leg->SetTextFont(42);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->SetBorderSize(0);
   leg->Draw();

   if (Logy) data_mc_canvas->SetLogy();
   pad2->cd();                                                                     gPad->SetTopMargin(0);
   gPad->SetBottomMargin(0.2);
   gPad->SetLeftMargin(0.10); gPad->SetRightMargin(0.03);
   
   TH1F* hdif1=(TH1F*)h1->Clone();
    TH1F* hdif2=(TH1F*)h2->Clone();
     hdif1->SetTitle(" ");
    hdif1->GetYaxis()->SetTitle("#Delta ("+leg1+","+leg2+")");
    hdif1->GetYaxis()->SetLabelSize(0.07);
    hdif1->GetYaxis()->SetTitleSize(0.1);
    hdif1->GetYaxis()->SetTitleOffset(0.4);
   hdif1->GetXaxis()->SetLabelSize(0.1);
    hdif1->GetXaxis()->SetTitleSize(0.1);
    hdif1->GetXaxis()->SetTitleOffset(0.7);

    hdif1->Add(hdif2,-1);
    hdif1->Draw("P");
    hdif1->SetMarkerStyle(kFullCircle);
   hdif1->SetMarkerSize(1);
   return data_mc_canvas;

   }


TCanvas *canvas_3plot(TH1F * h1, TH1F * h2, TH1F * h3, TString canvas, bool Logy, bool norm,double miny, double maxy, TString Labelx,TString Labely, TString leg1,TString leg2,TString leg3){

  TCanvas *data_mc_canvas = new TCanvas("DATA_MC_"+canvas,"DATA_MC_"+canvas,700,700);
    gStyle->SetPadBorderMode(0);
  gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);

  h1->SetLineColor(kRed);
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  h2->SetLineColor(kBlue);
  h3->SetLineWidth(2);
  h3->SetLineColor(kBlack);

 
  h1->GetYaxis()->SetTitle(Labely);
  h1->GetXaxis()->SetTitle(Labelx);
 // h1->GetYaxis()->SetTitleSize(0.06);
 // h1->GetYaxis()->SetTitleOffset(0.8);
 // h1->GetYaxis()->SetLabelSize(0.05);
 // h1->GetXaxis()->SetTitleSize(0.09);

  if (norm) {
      h1->Scale(1/h1->Integral());
      h2->Scale(1/h2->Integral());
      h3->Scale(1/h3->Integral());
   }

  h1->Draw("HIST ");
  h2->Draw("HIST sames");
  if(leg3!="data")
  h3->Draw("HIST sames");
  else
   h3->Draw("sames");

    if(miny!=-1 && maxy!=-1)
  h1->GetYaxis()->SetRangeUser(miny,maxy);
  
   TLegend * leg = new TLegend(0.70,0.70,1,1);
   //leg->AddEntry(data_xs,"#font[62]{CMS Preliminary}", "");
   //leg->AddEntry(data_xs,"#font[62]{#font[12]{L}~0.6 fb^{-1}}", "");
   //leg->AddEntry(data_xs,"Run = 304366", "");
   leg->AddEntry(h1,leg1);
   leg->AddEntry(h2,leg2);
   leg->AddEntry(h3,leg3);
   leg->SetTextFont(42);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->SetBorderSize(0);
   leg->Draw();

   if (Logy) data_mc_canvas->SetLogy();

   return data_mc_canvas;

   }





TCanvas *canvas_2d(TH2F * h2,TString name, TString xaxis, TString yaxis){
TCanvas * ctemp=new TCanvas(name,name,700,700);
gStyle->SetOptStat(0);
h2->Draw("COLZ");
h2->GetXaxis()->SetTitle(xaxis);
h2->GetYaxis()->SetTitle(yaxis);
return ctemp;
}

TCanvas * canvas_prof(TH1F * h1, TH1F * h2, TH1F * h3,double xax[],double xerr[], TString name, TString title){
double yax[3]={h1->GetMean(),h2->GetMean(),h3->GetMean()};
double yerr[3]={h1->GetStdDev(),h2->GetStdDev(),h3->GetStdDev()};
TGraphErrors * gr=new TGraphErrors(3,xax,yax,xerr,yerr);
TCanvas * ct= new TCanvas(name,name,700,700);
gr->SetTitle(title);
gr->Draw("*A");
return ct;
}

TCanvas * canvas_proj(TH2F * h1,TString name, TString titleX){
TCanvas * ct=new TCanvas("c"+name,"c"+name,700,700);
TH1D *hp1=h1->ProjectionY("h1"+name,0,1);
TH1D *hp2=h1->ProjectionY("h2"+name,1,2);
TH1D *hp3=h1->ProjectionY("h3"+name,2,3);

hp1->Draw("HIST");
hp2->Draw("HIST sames");
hp3->Draw("HIST sames");

hp1->SetLineColor(1);
hp2->SetLineColor(2);
//hp3->SetLineColor(3);

hp1->SetTitle("#font[22]{CMS} #font[12]{Preliminary};"+titleX);
hp1->SetLineWidth(2);
hp2->SetLineWidth(2);
hp3->SetLineWidth(2);

hp1->Scale(1/hp1->Integral());
hp2->Scale(1/hp2->Integral());
hp3->Scale(1/hp3->Integral());

return ct;
}


TCanvas * canvas_prof(TH2F * h1,TString name, TString title){
TCanvas * ct=new TCanvas("c"+name,"c"+name,700,700);
 TH1D *hp1=h1->ProfileX("h1"+name,1,-1,"s");
 hp1->Draw();

hp1->SetLineColor(1);
//hp3->SetLineColor(3);

hp1->SetTitle(title);
hp1->SetLineWidth(2);

return ct;
}

TCanvas * canvas_2prof(TH2F * h1,TH2F * h2,TString name, TString titleX, TString titleY){
TCanvas * ct=new TCanvas("c"+name,"c"+name,700,700);
 TH1D *hp1=h1->ProfileX("h1"+name,1,-1,"s");
 TH1D *hp2=h2->ProfileX("h2"+name,1,-1,"s");
 hp1->Draw();
 hp2->Draw("sames");

hp1->SetLineColor(1);
hp2->SetLineColor(2);

hp1->SetTitle("#font[22]{CMS} #font[12]{Preliminary};"+titleX+";"+titleY);
hp1->SetLineWidth(2);
hp2->SetLineWidth(2);

return ct;
}
