void analysis1()
{
    TFile *file = new TFile("results/analysis1.root");
    TTree *tree = (TTree*)file->Get("analysis1");

    TCanvas *c1 = new TCanvas("c1", "Analysis 1", 800, 600);

    c1->SetGrid();

    tree->SetMarkerStyle(20);
    tree->SetMarkerSize(0.8);
    tree->SetMarkerColor(kBlue);

    tree->Draw("warm_days:year", "", "P");

    tree->GetHistogram()->SetTitle("Summer days >= 25 C in Lund");
    tree->GetHistogram()->GetXaxis()->SetTitle("Year");
    tree->GetHistogram()->GetYaxis()->SetTitle("Number of days (June-August)");
    c1->SaveAs("results/analysis1.pdf");
}
