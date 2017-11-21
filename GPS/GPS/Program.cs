using Ghostware.NMEAParser;
using Ghostware.NMEAParser.NMEAMessages;
using InTheHand.Net;
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace GPS
{
    class Program
    {
        static string desiredDeviceName = "PENTA-GPS";
        static BluetoothClient bluetoothClient = new BluetoothClient();
        static void Main(string[] args)
        {
            Console.WriteLine("Detecting devices ...");
            BluetoothDeviceInfo deviceToPair = null;
            var devices = bluetoothClient.DiscoverDevices();
            deviceToPair = devices.Where(x => x.DeviceName.Equals(desiredDeviceName)).FirstOrDefault();
            if (deviceToPair != null)
            {
                Console.WriteLine(desiredDeviceName + " found!");
                Console.WriteLine(string.Format(
                    "DeviceName: {0}\nAddress: {1}\n",
                    deviceToPair.DeviceName, deviceToPair.DeviceAddress));
                Console.WriteLine();
                deviceToPair.Update();
                deviceToPair.Refresh();
                deviceToPair.SetServiceState(BluetoothService.ObexObjectPush, true);
                bool isPaired = BluetoothSecurity.PairRequest(deviceToPair.DeviceAddress, "0000");
                if (isPaired)
                {
                    Console.WriteLine("Device paired, requesting data");
                    bluetoothClient.BeginConnect(deviceToPair.DeviceAddress, BluetoothService.SerialPort, new AsyncCallback(Connect), deviceToPair);
                }
                else
                {
                    Console.WriteLine("Device not paired, check bluetooth connection and PIN validity ");
                }
            }
            else
            {
                Console.WriteLine(string.Format("Found {0} devices but no {1} among them",
                       devices.Length, desiredDeviceName));
                    
            }
            Console.ReadKey();
        }

        private static void Connect(IAsyncResult result)
        {
            if (result.IsCompleted)
            {
                bool finished = false; // keep it false until we read what we needed
                var myNetworkStream = bluetoothClient.GetStream();
                while (!finished)
                {
                    if (myNetworkStream.CanRead)
                    {
                        byte[] myReadBuffer = new byte[256];
                        StringBuilder myCompleteMessage = new StringBuilder();
                        int numberOfBytesRead = 0;

                        // Incoming message may be larger than the buffer size. 
                        do
                        {
                            numberOfBytesRead = myNetworkStream.Read(myReadBuffer, 0, myReadBuffer.Length);

                            myCompleteMessage.AppendFormat("{0}", Encoding.ASCII.GetString(myReadBuffer, 0, numberOfBytesRead));
                                                    }
                        while (myNetworkStream.DataAvailable);
                        try
                        {
                            NmeaParser parser = new NmeaParser();
                            GpggaMessage parsedMessage = (GpggaMessage)parser.Parse(myCompleteMessage.ToString());
                            Console.WriteLine("You received the following message : " +
                                                     myCompleteMessage.ToString());
                            ShowParsedMessage(parsedMessage);
                            finished = true;
                        }
                        catch (Exception exc)
                        {
                            Console.WriteLine("ERROR, wrong message read, trying again");
                            finished = false;
                        }
                    }
                    else
                    {
                        Console.WriteLine("Sorry.  You cannot read from this NetworkStream.");
                    }
                }
            }
        }

        public static void ShowParsedMessage(GpggaMessage parsedMessage)
        {
            double minutes = (parsedMessage.Latitude - Math.Floor(parsedMessage.Latitude)) * 60.0;
            double seconds = (minutes - Math.Floor(minutes)) * 60.0;
            double tenths = (seconds - Math.Floor(seconds)) * 10.0;
            // get rid of fractional part
            minutes = Math.Floor(minutes);
            seconds = Math.Floor(seconds);
            tenths = Math.Floor(tenths);
            Console.WriteLine("Latitude: " + Math.Floor(parsedMessage.Latitude) + "°" + minutes + "'" + seconds + "." + tenths + "\"" + "E");
            minutes = (parsedMessage.Longitude - Math.Floor(parsedMessage.Longitude)) * 60.0;
            seconds = (minutes - Math.Floor(minutes)) * 60.0;
            tenths = (seconds - Math.Floor(seconds)) * 10.0;
            // get rid of fractional part
            minutes = Math.Floor(minutes);
            seconds = Math.Floor(seconds);
            tenths = Math.Floor(tenths);
            Console.WriteLine("Longitude: " + Math.Floor(parsedMessage.Longitude) + "°" + minutes + "'" + seconds + "." + tenths + "\"" + "N");
            Console.WriteLine("Number of satellites being tracked: " + parsedMessage.NumberOfSatellites);
            Console.WriteLine("Altitude above sea level " + parsedMessage.Altitude + "m");
            Console.WriteLine(parsedMessage.Latitude.ToString() + ", " + parsedMessage.Longitude.ToString());


        }
    }
}