/*
 *  Cypress -- C++ Spiking Neural Network Simulation Framework
 *  Copyright (C) 2018  Andreas Stöckel, Christoph Ostrau
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file simple.cpp
 *
 * A simple network consisting of a source and a target neuron. The source
 * neuron is connected to the target neuron using an one-to-one projection.
 * Spikes in both the source and target neuron populations are recorded and
 * printed at the end of the program.
 *
 * @author Christoph Ostrau
 */
#include <cypress/cypress.hpp>

#include <fstream>
#include <iostream>

using namespace cypress;

double runtime = 100.0;  // Simulation runtime in ms

int main(int argc, const char *argv[])
{
	if (argc != 2 && !NMPI::check_args(argc, argv)) {
		std::cout << "Usage: " << argv[0] << " <SIMULATOR>" << std::endl;
		return 1;
	}

	global_logger().min_level(LogSeverity::INFO);

	// Reading the configuration file
	std::ifstream ifs("../config/parameters.json");
	if (!ifs.good()) {
		throw std::runtime_error("Could not open config file");
	}
	Json config;
	ifs >> config;

	// Parsing NeuronParameters
	NeuronParameter neuro_params(IfCondExp::inst(), config["neuron_params"]);

	// Create the network
	auto netw = Network();

	// Create a population of 1 neuron, record the membrane voltage, synaptic
	// conductance and spikes. Use Neuron Parameters from config files
	auto pop = netw.create_population<IfCondExp>(
	    1, neuro_params.parameter(),
	    IfCondExpSignals({"v", "spikes", "gsyn_exc"}));

	// Definition of source spike times
	std::vector<cypress::Real> spike_times({50.0});

	// Create spike source population with one virtual neuron
	auto pop_source = netw.create_population<SpikeSourceArray>(
	    1, SpikeSourceArrayParameters(spike_times),
	    SpikeSourceArraySignals({"spikes"}));

	// Connect populations using a one-to-one connection, all connections share
	// the same weight, and a delay of 1.0 ms
	netw.add_connection(
	    pop_source, pop,
	    Connector::one_to_one(cypress::Real(config["weight"]), 1.0));

	// Run the simulation for runtime milliseconds, argv[1] is the first command
	// line argument and should contain the simulator name
	netw.run(argv[1], runtime, argc, argv);

	// Print the spike times for each source neuron
	for (size_t i = 0; i < pop_source.size(); i++) {
		std::cout << "Spike times for source neuron " << i << std::endl;
		std::cout << pop_source[i].signals().data(0);
	}

	// Print the spike times for each target neuron
	std::vector<std::vector<Real>> spikes;
	for (size_t i = 0; i < pop.size(); i++) {
		std::cout << "Spike times for target neuron " << i << std::endl;
		std::cout << pop[i].signals().data(0);
		spikes.push_back(pop[i].signals().get_spikes());
	}

	// Plot membrane voltage and spike times
	auto v_and_time = pop.signals().get_v();
	auto g_and_time = pop.signals().get_gsyn_exc();
	std::vector<Real> time, voltage, gsyn;
	for (size_t i = 0; i < v_and_time.rows(); i++) {
		time.push_back(v_and_time(i, 0));
		voltage.push_back(v_and_time(i, 1));
		gsyn.push_back(g_and_time(i, 1));
	}

	pyplot::figure_size(600, 800);
	pyplot::subplot(3, 1, 1);
	pyplot::plot(time, voltage);
	pyplot::title("Membrane voltage for simulator " + std::string(argv[1]));
	pyplot::xlabel("Time in ms");
	pyplot::ylabel("Voltage in mV");

	pyplot::subplot(3, 1, 2);
	pyplot::plot(time, gsyn);
	pyplot::title("Conductance for simulator " + std::string(argv[1]));
	pyplot::xlabel("Time in ms");
	pyplot::ylabel("Conductance in mS");

	pyplot::subplot(3, 1, 3);
	pyplot::eventplot(spikes);
	pyplot::title("Spike Times");
	pyplot::xlabel("Time in ms");
	pyplot::ylabel("Neuron ID");
	pyplot::xlim(0, int(runtime));
	if (spikes.size() > 1) {
		pyplot::ylim(-0.5, spikes.size() - 0.5);
	}
	else {
		pyplot::ylim(0.5, 1.5);
	}
	pyplot::tight_layout();
	pyplot::show();

	return 0;
}
