//
// Calculate gross pay, net pay, pension and deductions for employees of Rogers Hostpital Supplies Company.
//
// Bob Langelaan
// March 9, 2015
//

#include <iostream>
#include <conio.h>  // Required for _getch() function
#include <fstream>  // Required for file input/output
#include <cmath>
#include <iomanip> 
#include <string>

using namespace std;

//global constants

// One way to specify a file name:
const char * IN_FILE = "EmployeePayInput.txt";

// A second way to specify a file name:
#define OUT_FILE "EmployeePayOutput.txt"

//TODO: Neet to add consts for calculations and change the variables names in th emain to more meaningfull descripiton
const double TAXABLE_MIN = 0.0;
const double MIN_PENSION_WITHHOLDING = 16.5;
const int MIN_WEEKLY_HOURS = 0;
const int MAX_WEEKLY_HOURS = 54;
const int STANDARD_WEEK_HOURS = 40;
const double MIN_PAY_RATE = 0.0;
const double MAX_PAY_RATE = 99.99;
const int MIN_EXEMPTIONS = 0;
const int MAX_EXEMPTIONS = 19;
string printFileHeader = "Rogers Hospital Supplies Company\n---------------------------------\n\n";
string printGridHeader = "Social insurance no.   gross pay    net pay    pension    deductions\n--------------------------------------------------------------------\n";

int employeesProcessed = 0;
double grossPayTotal = 0.0;
double netPayTotal = 0.0;
double pensionTotal = 0.0;
double deductionsTotal = 0.0;

double calculateGrossPay(double hours, double payRate);
bool isValid(long sin, double pr, int expt, double hr);
double calculateTaxable(double gross, int exempts);
double calculateFederal(double taxable);
double calculateProvincial(double federal);
double calculatePensionWithholdings(double gross);
double calculateNetPay(double gross, double federal, double provincial, double pension);
double round_2nPlaces(double val, int n_places);


int main()
{
	//Declare variables
	long socialInsuranceNum;  // Will store social security number of employee
	int numberOfExemptions;  // Will store number of excemptions for employee

	double payRate,			// Will store the pay rate for the employee
		hoursWorked;		// Will store hours worked for the employee

	//Define ifstream object and open file
	ifstream ins;
	ins.open(IN_FILE);

	//Check that file opened without any issues
	if (ins.fail())
	{
		cerr << "ERROR--> Unable to open input file : " << IN_FILE << endl;
		cerr << '\n' << endl;
		return -1; //error return code
	}

	//Define ofstream object and open file
	ofstream outs;
	outs.open(OUT_FILE);

	//Check that file opened without any issues
	if (outs.fail())
	{
		cerr << "ERROR--> Unable to open output file : " << OUT_FILE << endl;
		cerr << '\n' << endl;
		return -2; //error return code
	}

	outs << printFileHeader;
	outs << printGridHeader;

	// Process data until end of file is reached
	while (!ins.eof())
	{
		// Read in other data for the employee
		ins >> socialInsuranceNum >> payRate >> numberOfExemptions >> hoursWorked;

		if (!isValid(socialInsuranceNum,payRate,numberOfExemptions,hoursWorked)) 
		{
			continue;
		}

		employeesProcessed++;


		double grossPay = calculateGrossPay(hoursWorked, payRate);
		grossPayTotal += grossPay;
		double taxable = calculateTaxable(grossPay, numberOfExemptions);
		
		if (taxable <= TAXABLE_MIN) 
		{
			taxable = TAXABLE_MIN;
		}

		double federal = calculateFederal(taxable);
		double provincial = calculateProvincial(federal);
		
		double pensionWithholdings = calculatePensionWithholdings(grossPay);
		pensionTotal += pensionWithholdings;
		
		//federal + provincial + pensionWithholdings;
		double allDeductions = round_2nPlaces(federal + provincial + pensionWithholdings, 2);
		deductionsTotal += allDeductions;
		
		double netPay = calculateNetPay(grossPay, federal, provincial, pensionWithholdings);
		netPayTotal += netPay;

		outs << setw(20) << left << socialInsuranceNum << setprecision(2) << fixed << 
					 setw(12) << right << grossPay << 
					 setw(11) << right << netPay << 
					 setw(11) << right << pensionWithholdings << 
					 setw(14) << right << allDeductions;
		outs << endl;
	}

	outs << endl;
	outs << endl;

	outs << "Summary\n------\n\n";
	outs << setw(45) << left << "Number of employees processed:" << setw(7) << right << employeesProcessed << endl;
	outs << setw(45) << left << "Total gross pay for all employees:" << setw(7) << right << setprecision(2) << fixed << grossPayTotal << endl;
	outs << setw(45) << left << "Total net pay for all employees:" << setw(7) << right << netPayTotal << endl;
	outs << setw(45) << left << "Total pension withheld for all employees:" << setw(7) << right << pensionTotal << endl;
	outs << setw(45) << left << "Total deductions for all employees:" << setw(7) << right << deductionsTotal << endl;

	// Close files
	ins.close();
	outs.close();

	cout << '\n' << endl;
}

double calculateGrossPay(double hours, double payRate) 
{
	double grossPay;
	if (hours <= STANDARD_WEEK_HOURS)
	{
		grossPay = hours * payRate;
	} 
	else
	{
		double stdPay = STANDARD_WEEK_HOURS * payRate;
		double extraHours = hours - STANDARD_WEEK_HOURS;
		double extraHoursPay = extraHours * (payRate + (payRate / 2));
		grossPay = stdPay + extraHoursPay;
	}

	return round_2nPlaces(grossPay, 2);
}

double calculateFederal(double taxable)
{
	//federal = taxable * (0.14 + (0.00023  * taxable))
	double tempFederal = taxable * (0.14 + (0.00023 * taxable));
	return round_2nPlaces(tempFederal, 2);
}

double calculateTaxable(double gross, int exempts) 
{
	//taxable = gross - ($14.00 * exempt) - $11.00
	double tempTaxable = gross - (14.00 * exempts) - 11.0;
	return round_2nPlaces(tempTaxable, 2);
}

double calculateProvincial(double federal)
{
	double tempFederal= (federal * 35) / 100;
	return round_2nPlaces(tempFederal, 2);
}

double calculatePensionWithholdings(double gross)
{
	//16.5 or 7.7% of gross whichever is lower;
	double pensionWithholding = (gross * 7.7) / 100;
	double pwh2digits = round_2nPlaces(pensionWithholding,2);
	return (MIN_PENSION_WITHHOLDING < pwh2digits) ? MIN_PENSION_WITHHOLDING : pwh2digits;	
}

double calculateNetPay(double gross, double federal, double provincial, double pension)
{
	//Net Pay = GrossPay - All deductions;
	double tempGross = gross - (federal + provincial + pension);
	return round_2nPlaces(tempGross, 2);
}

bool isValid(long sin, double pr, int expt, double hw) 
{
	bool totalValidationResult = true;
	bool results[] = {true,true,true,true};  // 4 validations in total;
	
	//validating social insurance #;
	auto temp = to_string(sin);
	if (9 == temp.length())
	{
		results[0] = true;
	}
	else
	{
		results[0] = false;
		cerr << setw(10) << sin << "\tINPUT DATA ERROR:\tInvalid social security #" << endl;
	}

	//check pay rate;
	if ((pr >= MIN_PAY_RATE) & (pr <= MAX_PAY_RATE))		//bitwise comparison => no shortcircutting; 
 	{
		results[1] = true;
	}
	else 
	{
		results[1] = false;
		cout.precision(2);
		cerr << setw(10) << sin << "\tINPUT DATA ERROR:\tInvalid pay rate: " << pr << endl;
	}

	//check exemptions; min 0 max 19
	if ((expt >= MIN_EXEMPTIONS) & (expt <= MAX_EXEMPTIONS))		//bitwise comparison => no shortcircutting; 
	{
		results[2] = true;
	}
	else
	{
		results[2] = false;
		cerr << setw(10) <<sin << "\tINPUT DATA ERROR:\tInvalid number of exemptions: " << expt << endl;
	}

	//check hours worked => no more than 54 hours a week;
	if ((hw >= MIN_WEEKLY_HOURS) & (hw <= MAX_WEEKLY_HOURS))
	{
		results[3] = true;
	}
	else
	{
		results[3] = false;
		cout.precision(2);
		cerr << setw(10) << sin << "\tINPUT DATA ERROR:\tInvalid hours worked: " << hw << endl;
	}

	for (bool i : results)
	{
		if (!i) //if any of the validation fails => exit;
		{
			totalValidationResult = false;
			break;
		}
	}
	return totalValidationResult;
}

double round_2nPlaces(double val, int n_places)
{
	double result = floor(val * 100 + 0.5) / 100;
	return result;
}

