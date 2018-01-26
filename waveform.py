import ROOT
from array import array
import argparse

def main(args):
    adc_data_file = ROOT.TFile(args.input_file)

    adc_data = adc_data_file.Get("adc_data") 
    adc_data.GetEntry(args.entry)
    header_tree = adc_data_file.Get("nevis_header") 
    header_tree.GetEntry(args.entry)

    branchname = "channel_%d_adc_words" % args.channel
    data = getattr(adc_data, branchname)

    graph_title = "Event %i Channel %i Waveform" % (header_tree.event_number, args.channel)
    plot(data, args.output, graph_title)

def plot(adc_data, output_name, graph_title):
    n_data = len(adc_data)

    adc_data_array = array('i')
    time_array = array('i')
    for i,d in enumerate(adc_data):
        adc_data_array.append(d)
        time_array.append(i)
    
    canvas = ROOT.TCanvas("canvas", "Waveform Canvas", 250,100,700,500)

    graph = ROOT.TGraph(n_data, time_array, adc_data_array)
    graph.SetTitle(graph_title)
    graph.GetXaxis().SetTitle("adc number")
    graph.GetYaxis().SetTitle("adc value")
    graph.Draw()

    canvas.Update()
    canvas.SaveAs(output_name + ".pdf")
    

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input_file", default="output.root")
    parser.add_argument("-o", "--output", default="waveform")
    parser.add_argument("-c", "--channel", type=int, default=0)
    parser.add_argument("-e", "--entry", type=int, default=0)
    
    main(parser.parse_args())
