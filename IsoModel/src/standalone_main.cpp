/*
 * standalone_main.cpp
 *
 *  Created on: Dec 5, 2014
 *      Author: nick
 */

#include "UserModel.hpp"
#include "SimModel.hpp"
#include <iostream>

#include <boost/program_options.hpp>

using namespace openstudio::isomodel;
using namespace openstudio;

void runMonthlySimulation(const UserModel& umodel) {
  // Run the monthly simulation.
  openstudio::isomodel::SimModel simModel = umodel.toSimModel();
  ISOResults results = simModel.simulate();

  std::cout << "Monthly Results:" << std::endl;
  std::cout
    << "Month,ElecHeat,ElecCool,ElecIntLights,ElecExtLights,ElecFans,ElecPump,ElecEquipInt,ElecEquipExt,ElectDHW,GasHeat,GasCool,GasEquip,GasDHW"
    << std::endl;
  for (int i = 0; i < 12; i++) {
    std::cout << i + 1;
#ifdef _OPENSTUDIOS
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorEquipment);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems);

    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment);
    std::cout << ", " << results.monthlyResults[i].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems);
#else
    for (int j = 0; j < 13; j++) {
      std::cout << ", " << results.monthlyResults[i].getEndUse(j);
    }
#endif
    std::cout << std::endl;
  }
}

void runHourlySimulation(const UserModel& umodel, bool aggregateByMonth) {
  // Run the hourly simulation (with results aggregated by month).
  openstudio::isomodel::ISOHourly hourly = umodel.toHourlyModel();
  ISOResults hourlyResults = hourly.calculateHourly(aggregateByMonth);

  std::string monthOrHour = aggregateByMonth ? "month" : "hour";
  int numberOfResults = aggregateByMonth ? 12 : 8760;

  // Output the results.
  std::cout << "Hourly results by " << monthOrHour << ":" << std::endl;
  std::cout
    << monthOrHour << ",ElecHeat,ElecCool,ElecIntLights,ElecExtLights,ElecFans,ElecPump,ElecEquipInt,ElecEquipExt,ElectDHW,GasHeat,GasCool,GasEquip,GasDHW"
    << std::endl;
  for (int i = 0; i < numberOfResults; i++) {
    std::cout << i + 1;
#ifdef _OPENSTUDIOS
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Heating);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Cooling);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorLights);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorLights);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Fans);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::Pumps);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::InteriorEquipment);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::ExteriorEquipment);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Electricity, EndUseCategoryType::WaterSystems);

    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Heating);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::Cooling);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::InteriorEquipment);
    std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(EndUseFuelType::Gas, EndUseCategoryType::WaterSystems);
#else
    for (int j = 0; j < 13; j++) {
      std::cout << ", " << hourlyResults.hourlyResults[i].getEndUse(j);
    }
#endif
    std::cout << std::endl;
  }
}

int main(int argc, char* argv[])
{
  namespace po = boost::program_options; 
  po::options_description desc("Options"); 
  desc.add_options()
    ("ismfilepath,i", po::value<std::string>()->required(), "Path to ism file.")
    ("monthly,m", "Run the monthly simulation (default).")
    ("hourlyByMonth,h", "Run the hourly simulation (results aggregated by month.")
    ("hourlyByHour,H", "Run the hourly simulation (results for each hour).");

  po::positional_options_description positionalOptions; 
  positionalOptions.add("ismfilepath", 1); 

  po::variables_map vm;

  try {
    po::store(po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), vm); // Throws on error. 
    po::notify(vm); // Throws if required options are mising.
  }
  catch(boost::program_options::required_option& e) 
  { 
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
    std::cerr << desc << std::endl;
    return 1; 
  } 
  catch(boost::program_options::error& e) 
  { 
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
    std::cerr << desc << std::endl;
    return 1; 
  } 

  if (DEBUG_ISO_MODEL_SIMULATION) {
    std::cout << "Loading User Model..." << std::endl;
  }

  // Load the .ism file.
  openstudio::isomodel::UserModel umodel;
  umodel.load(vm["ismfilepath"].as<std::string>());

  if (DEBUG_ISO_MODEL_SIMULATION)
    std::cout << "User Model Loaded" << std::endl;

  if (DEBUG_ISO_MODEL_SIMULATION) {
    umodel.loadWeather();
    WeatherData wd = *umodel.weatherData();
    Matrix msolar = wd.msolar();
    Matrix mhdbt = wd.mhdbt();
    Matrix mhEgh = wd.mhEgh();
    Vector mEgh = wd.mEgh();
    Vector mdbt = wd.mdbt();
    Vector mwind = wd.mwind();
    printMatrix("msolar", msolar);
    printMatrix("mhdbt", mhdbt);
    printMatrix("mhEgh", mhEgh);
    printVector("mEgh", mEgh);
    printVector("mdbt", mdbt);
    printVector("mwind", mwind);
    std::cout << std::endl;
  }

  bool simulationRan = false;

  if (vm.count("monthly")) {
    runMonthlySimulation(umodel);
    simulationRan = true;
  }

  if (vm.count("hourlyByMonth")) {
    runHourlySimulation(umodel, true);
    simulationRan = true;
  }

  if (vm.count("hourlyByHour")) {
    runHourlySimulation(umodel, false);
    simulationRan = true;
  }

  if (!simulationRan) {
    // Monthly simulation is default and will run if no other simulations did.
    runMonthlySimulation(umodel);
  }

}

