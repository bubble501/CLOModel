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
            double num = 0F; // TODO: Initialize to an appropriate value
            uint precision = 0; // TODO: Initialize to an appropriate value
            string expected = string.Empty; // TODO: Initialize to an appropriate value
            string actual;
            actual = ABSUtilities_Accessor.FormatNumber(num, precision);
            Assert.AreEqual(expected, actual);
            Assert.Inconclusive("Verify the correctness of this test method.");
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
            string a = string.Empty; // TODO: Initialize to an appropriate value
            bool expected = false; // TODO: Initialize to an appropriate value
            bool actual;
            actual = ABSUtilities_Accessor.IsBloombergVector(a);
            Assert.AreEqual(expected, actual);
            Assert.Inconclusive("Verify the correctness of this test method.");
        }

        /// <summary>
        ///A test for MoveBloombergVector
        ///</summary>
        [TestMethod()]
        [DeploymentItem("NETapi.dll")]
        public void MoveBloombergVectorTest()
        {
            string OriginalVect = string.Empty; // TODO: Initialize to an appropriate value
            int ByMonths = 0; // TODO: Initialize to an appropriate value
            string expected = string.Empty; // TODO: Initialize to an appropriate value
            string actual;
            actual = ABSUtilities_Accessor.MoveBloombergVector(OriginalVect, ByMonths);
            Assert.AreEqual(expected, actual);
            Assert.Inconclusive("Verify the correctness of this test method.");
        }


        /// <summary>
        ///A test for UnpackBloombergVect
        ///</summary>
        [TestMethod()]
        [DeploymentItem("NETapi.dll")]
        public void UnpackBloombergVectTest()
        {
            string Vect = string.Empty; // TODO: Initialize to an appropriate value
            int PaymFreq = 0; // TODO: Initialize to an appropriate value
            bool AdjustFreq = false; // TODO: Initialize to an appropriate value
            List<double> expected = null; // TODO: Initialize to an appropriate value
            List<double> actual;
            actual = ABSUtilities_Accessor.UnpackBloombergVect(Vect, PaymFreq, AdjustFreq);
            Assert.AreEqual(expected, actual);
            Assert.Inconclusive("Verify the correctness of this test method.");
        }

        /// <summary>
        ///A test for pmt
        ///</summary>
        [TestMethod()]
        [DeploymentItem("NETapi.dll")]
        public void pmtTest()
        {
            double Inter = 0F; // TODO: Initialize to an appropriate value
            int Periods = 0; // TODO: Initialize to an appropriate value
            double PresentValue = 0F; // TODO: Initialize to an appropriate value
            double expected = 0F; // TODO: Initialize to an appropriate value
            double actual;
            actual = ABSUtilities_Accessor.pmt(Inter, Periods, PresentValue);
            Assert.AreEqual(expected, actual);
            Assert.Inconclusive("Verify the correctness of this test method.");
        }
    }
}
