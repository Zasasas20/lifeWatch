using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace App
{
    public class AppResponse
    {
        public string sessionID { get; set; }
        public string code { get; set; }
        public string status { get; set; }
        public double battery{  get; set; }
        public string req { get; set; }
    }

    public class SOS
    {
        public string code { get; set;}
        public string status { get; set; }
    }

    public class LocationData
    {
        public double Long { get; set; }
        public double Lat { get; set; }
    }

    public class ChipResponse
    {
        public string code { get; set; }
        public LocationData LocationData { get; set; }
        public string req { get; set; }
    }

    public class AppMessage
    {
        public string code { get; set; }
        public string message { get; set; }
    }
}
