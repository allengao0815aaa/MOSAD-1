using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Windows.Data.Json;
using Windows.Web.Http;

namespace Explore.Services
{
    class APIRequest
    {
        public static Uri baseUri = new Uri("http://api.avatardata.cn");
        public const string hiostoryToday = "HistoryToday/LookUp";
        public const string IPSearch = "IpLookUp/LookUp";
        private const string HT_KEY = "6b837f4edecc433eb0a4a4c542cd68f3";
        private const string IP_KEY = "60bd6632abef457ca728d204b1a60b1d";
        public const int XML = 0;
        public const int JSON = 1;
        public static int MAX_ROW = 10;

        public static string ParseQuery(NameValueCollection keyword, string type)
        {
            List<string> list = new List<string>
            {
                "key=" + (type == hiostoryToday ? HT_KEY : IP_KEY)
            };
            foreach (string key in keyword.AllKeys)
            {
                list.Add(key + "=" + keyword[key]);
            }
            return string.Join("&", list);
        }

        public static Uri ParseUri(NameValueCollection keyword, string type)
        {
            UriBuilder builder = new UriBuilder(baseUri);
            if (type == hiostoryToday)
            {
                if (keyword.Count < 3) return null;
                builder.Path = hiostoryToday;
            } 
            else
            {
                if (keyword.Count < 1) return null;
                builder.Path = IPSearch;
            }
            builder.Query = ParseQuery(keyword, type);
            return builder.Uri;
        }

        public static async Task<string> GetData(NameValueCollection keyword, string type, int dataType)
        {
            if (dataType != JSON && dataType != XML)
                return string.Empty;

            HttpClient httpClient = new HttpClient();
            var headers = httpClient.DefaultRequestHeaders;
            string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36";
            if (!headers.UserAgent.TryParseAdd(userAgent))
                return string.Empty;

            Uri target = ParseUri(keyword, type);
            if (target == null)
                return string.Empty;

            HttpResponseMessage httpResponse = new HttpResponseMessage();
            string responseBody = string.Empty;

            try
            {
                httpResponse = await httpClient.GetAsync(target);
                httpResponse.EnsureSuccessStatusCode();
                responseBody = await httpResponse.Content.ReadAsStringAsync();
            }
            catch (Exception ex)
            {
                responseBody = "Error: " + ex.HResult.ToString("X") + " Message: " + ex.Message;
            }
            if (dataType == JSON)
                return ParseJSONResult(responseBody);
            else
                return ParseXMLResult(responseBody);
        }

        private static string ParseXMLResult(string responseBody)
        {
            XmlDocument xml = new XmlDocument();
            StringBuilder res = new StringBuilder();
            xml.LoadXml(responseBody);
            if (xml.GetElementsByTagName("error_code").Item(0).InnerText != "0")
                return string.Empty;
            var resNode = xml.GetElementsByTagName("result").Item(0);
            var childs = resNode.ChildNodes;
            foreach (XmlNode node in childs)
            {
                res.AppendFormat("{0, 10}:\t{1}\n", node.Name, node.InnerText);
            }
            return res.ToString();
        }

        public static string ParseJSONResult(string ori)
        {
            JsonObject pairs = JsonObject.Parse(ori);
            StringBuilder res = new StringBuilder();
            if (pairs.GetNamedNumber("error_code") != 0)
                return string.Empty;
            var temp = pairs.GetNamedArray("result");
            foreach (var value in temp)
            {
                var obj = value.GetObject();
                string date = obj.GetNamedNumber("year").ToString()
                    + "-" + obj.GetNamedNumber("month").ToString() + "-" + obj.GetNamedNumber("day").ToString() + " ";
                res.AppendFormat("{0}\t{1}\n", date, obj.GetNamedString("title"));
            }
            return res.ToString();
        }
    }
}
