using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;

namespace APIUnitTest
{
    
    
    /// <summary>
    ///This is a test class for ABSUtilitiesTest and is intended
    ///to contain all ABSUtilitiesTest Unit Tests
    ///</summary>
    [TestClass()]
    public class ABSUtilitiesTest
    {


        private TestContext testContextInstance;

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region Additional test attributes
        // 
        //You can use the following additional attributes as you write your tests:
        //
        //Use ClassInitialize to run code before running the first test in the class
        //[ClassInitialize()]
        //public static void MyClassInitialize(TestContext testContext)
        //{
        //}
        //
        //Use ClassCleanup to run code after all tests in a class have run
        //[ClassCleanup()]
        //public static void MyClassCleanup()
        //{
        //}
        //
        //Use TestInitialize to run code before running each test
        //[TestInitialize()]
        //public void MyTestInitialize()
        //{
        //}
        //
        //Use TestCleanup to run code after each test has run
        //[TestCleanup()]
        //public void MyTestCleanup()
        //{
        //}
        //
        #endregion


        /// <summary>
        ///A test for DiscountMargin
        ///</summary>
        [TestMethod()]
        [DeploymentItem("NETapi.dll")]
        public void DiscountMarginTest()
        {
            List<DateTime> Dte = new List<DateTime>();
            Dte.Add(new DateTime(2014, 3, 10)); Dte.Add(new DateTime(2015, 3, 10)); Dte.Add(new DateTime(2016, 3, 10));
            List<double> Flws = new List<double>();
            Flws.Add(-100); Flws.Add(10); Flws.Add(105);
            int Daycount = 365;
            double Guess = 0.05;
            int precision = 3;
            double BaseRate = 0.003;
            double expected = ABSUtilities_Accessor.DiscountMarginold(Dte, Flws, BaseRate, Daycount, Guess, precision);
            double actual=ABSUtilities_Accessor.DiscountMargin(Dte, Flws, BaseRate, Daycount, Guess, precision);
            Assert.AreEqual(expected, actual, 0.009, "DM error");
        }

        /// <summary>
        ///A test for FormatNumber
        ///</summary>
        [TestMethod()]
        [DeploymentItem("NETapi.dll")]
        public void FormatNumberTest()
        {

            Assert.AreEqual<String>("1,234,567.9", ABSUtilities_Accessor.FormatNumber(1234567.89, 1));

        }

        /// <summary>
        ///A test for IRR
        ///</summary>
        [TestMethod()]
        [DeploymentItem("NETapi.dll")]
        public void IRRTest()
        {
            List<DateTime> Dte = new List<DateTime>();
            Dte.Add(new DateTime(2014, 3, 10)); Dte.Add(new DateTime(2015, 3, 10)); Dte.Add(new DateTime(2016, 3, 10));
            List<double> Flws = new List<double>();
            Flws.Add(-100); Flws.Add(10); Flws.Add(105);
            int Daycount = 365;
            double Guess = 0.05;
            int precision = 3;
            double expected = ABSUtilities_Accessor.IRRold(Dte, Flws, Daycount, Guess, precision);
            double actual= ABSUtilities_Accessor.IRR(Dte, Flws, Daycount, Guess, precision);
            Assert.AreEqual(expected, actual, 0.009, "IRR error");
        }

        /// <summary>
        ///A test for NPV
        ///</summary>
        [TestMethod()]
        [DeploymentItem("NETapi.dll")]
        public void NPVTest()
        {
            List<DateTime> Dte = new List<DateTime>();
            Dte.Add(new DateTime(2014, 3, 10)); Dte.Add(new DateTime(2015, 3, 10)); Dte.Add(new DateTime(2016, 3, 10));
            List<double> Flws = new List<double>();
            Flws.Add(-100); Flws.Add(10); Flws.Add(105);
            double Interest = 0.05;
            int Daycount = 365;
            double expected = ABSUtilities_Accessor.NPVold(Dte, Flws, Interest, Daycount);
            double actual = ABSUtilities_Accessor.NPV(Dte, Flws, Interest, Daycount);
            Assert.AreEqual(expected, actual, 0.009, "NPV error");
        }

        /// <summary>
        ///A test for IsBloombergVector
        ///</summary>
        [TestMethod()]
        [DeploymentItem("NETapi.dll")]
        public void IsBloombergVectorTest()
        {
            Assert.IsTrue(ABSUtilities_Accessor.IsBloombergVector("5 24S 6"));
            Assert.IsTrue(ABSUtilities_Accessor.IsBloombergVector("5 24R  6"));
            Assert.IsTrue(ABSUtilities_Accessor.IsBloombergVector("5 12S\t6"));
            Assert.IsFalse(ABSUtilities_Accessor.IsBloombergVector("5 12U 6"));
            Assert.IsFalse(ABSUtilities_Accessor.IsBloombergVector("Ciao"));
        }


   

        /// <summary>
        ///A test for pmt
        ///</summary>
        [TestMethod()]
        [DeploymentItem("NETapi.dll")]
        public void pmtTest()
        {
            Assert.AreEqual(12950.45,ABSUtilities_Accessor.pmt(0.05, 10, 100000),0.01);
        }
    }
}
