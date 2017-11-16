using System;
using System.IO;
using System.Xml;
using System.Drawing;
using System.Drawing.Imaging;

namespace Image_XML_Conversion
{
    class Program
    {
        static void Main(string[] args)
        {
            //Grab image file and converting it into an array of bytes
            byte[] imgBytes = File.ReadAllBytes(@"C:\Users\Jonathan Ong\Desktop\apple.png");
            //Convert array into string
            string imgString = Convert.ToBase64String(imgBytes);
            //Set up xmlWriter settings for indentation
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.IndentChars = ("\t");
            settings.OmitXmlDeclaration = true;
            //Write image string into xml file
            using (XmlWriter writer = XmlWriter.Create("apple.xml", settings))
            {
                writer.WriteStartDocument();
                writer.WriteStartElement("Image2XML");
                writer.WriteStartElement("Image");
                writer.WriteElementString("byteString", imgString);
                writer.WriteEndElement();
                writer.WriteEndElement();
                writer.WriteEndDocument();
            }


            //Read in xml file of image
            using (XmlReader reader = XmlReader.Create("apple.xml"))
            {
                while (reader.Read())
                {
                    if (reader.IsStartElement())
                    {
                        switch (reader.Name)
                        {
                            //Skips other tags
                            case "byteString":
                                if (reader.Read())
                                {
                                    //Convert string back into byte array
                                    byte[] imgBytes2 = Convert.FromBase64String(reader.Value);
                                    //Put byte array into memorystream
                                    MemoryStream ms = new MemoryStream(imgBytes);
                                    //Create image from memorystream
                                    Image img = Image.FromStream(ms);
                                    //Save it as PNG
                                    img.Save("apple.png", ImageFormat.Png);
                                }
                                break;
                        }
                    }
                }
            }


        }
    }
}
