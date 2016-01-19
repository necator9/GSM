#include "functions.h"

#define DEVICE_PORT "/dev/ttyO4" 	// Path to serial port on BeagleBone Black

//Sending commands to device
string * send(string command)
{
	sleep(2);
	serialib LS;
		//cout << "inside of function" << endl;
		const char * comm = command.c_str();//conversion of input argument to char * type which is nessecary for seriallib
		int Ret;
		char Buffer[128];					// Define size of buffer (amount of symbols in SMS)
		Ret = LS.Open(DEVICE_PORT, 9600);   // Open serial link at 9600 bauds
		Ret = LS.WriteString(comm);         // Send the command on the serial port
		string chek_reading(comm);		// Conversion read value to string type
		cout << chek_reading << "  - recieved command" << endl;
		if (chek_reading == "AT+CMGR=1\n"){  		// Check for command on presence of reading sms command
			cout << "  - inside of condition" << endl;
			// Read a maximum of 128 characters with a timeout of 5 seconds
			// The final character of the string must be a line feed ('\n')
			int i = 0;
			string smsLineArray[3];
			while (i < 3) {
				Ret = LS.ReadString(Buffer,'\n',128,5000);	// Read first string from GSM module answer (sent command)
				cout << Buffer << "    - buffer" << endl;
				string buf(Buffer);
				smsLineArray[i] = buf;
				i++;
			}
			stringstream ss;
			ss << smsLineArray[1];
			string token, strContent1[4];
			i = 0;
				while(getline(ss, token, ',')) {
			    strContent1[i] = token;
			    i++;
			}
			ss.clear();
			ss << smsLineArray[2];
			i = 0;
			string strContent2[8];
			while(getline(ss, token, ' ')) {
				strContent2[i] = token;
			    i++;
			}
			string* retArray = new string[5];
			i = 0;
			while (i < 4){
				retArray[i] = strContent2[i];
				cout << retArray[i] << " - splitted"<< endl;;
				i++;
			}
			retArray[4] = strContent1[1];
			cout << retArray[4] << " - splitted"<< endl;
			sleep(2);
			LS.Close();
			return retArray;

		}
	sleep(2);
	LS.Close();				// Close the connection with the device
	return 0;
}

//Check on existence of file (uses for checking ADC and UART)
bool FileExists(const char *fname)
		{
		return ifstream(fname) != NULL;
		}

//ADC and serial port enabling
 void ADC_UART_enabling()
{
	 const char *fnameADC = "/sys/devices/ocp.3/helper.15";    	// Path to ADC directory for existing check
	 const char *fnameUART = "/dev/ttyO4";						// Path to UART directory for existing check
	 if (FileExists(fnameADC) == 0){							// Enable ADC in case if it was not enabled earlier
		 string adcarg = "cape-bone-iio";						// Enabling command
		 ofstream adc_on ("/sys/devices/bone_capemgr.9/slots"); // Path to configuration directory for ADC
		 adc_on << adcarg;										// Write command to file
		 adc_on.close();
		 cout << "ADC_enabled" << endl;
	}
	 if (FileExists(fnameUART) == 0){							// Enable UART in case if it was not enabled earlier
		 string UARTarg = "BB-UART4";							// Enabling command
		 ofstream UART_on ("/sys/devices/bone_capemgr.9/slots");// Path to configuration directory for ADC
		 UART_on << UARTarg;									// Write command to file
		 UART_on.close();
		 cout << "UART_enabled" << endl;
	 }
}

 //Reading and processing of ADC value to temperature
double T_ADC_reading()
{
	// ifstream adc_val1 ("/sys/devices/ocp.3/helper.15/AIN0");				// Path to file which contains value of ADC(0-1800)
	int n = 0;
	double T = 0;
	while (n < 1000){
		ifstream adc_val ("/sys/bus/iio/devices/iio\:device0/in_voltage0_raw"); // Path to file which contains value of ADC(0-4096)
		double adc_value;
		adc_val >> adc_value;													// Write ADC value to variable
		adc_value = 1.8 / 4095 * adc_value;										// Calculation
		double Rt = (816 * adc_value) / (1.8 - adc_value); 				// Calculation of resulted resistance
		T = T + (Rt - 1000) * 0.25;
		n ++;
	}
	T = T / 1000;
	return T;											// Return resulted temperature as argument
}

string convTtoMsg(double T){
	ostringstream strs;
	strs << T;
	string strT = strs.str();
	return strT;
}

