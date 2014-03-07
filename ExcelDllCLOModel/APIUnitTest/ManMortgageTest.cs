using ManagedCLO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;

namespace APIUnitTest
{
    
    
    /// <summary>
    ///This is a test class for ManMortgageTest and is intended
    ///to contain all ManMortgageTest Unit Tests
    ///</summary>
    [TestClass()]
    public class ManMortgageTest
    {
        [TestMethod]
        public void TestManMortgage()
        {
            ManMortgage LabRat=new ManMortgage();

            //Test failing ready to calculate
            Assert.AreEqual(LabRat.ReadyToCalculate(), "Loan Maturity Date");

            // Testing getting and setting properties
            DateTime TempMatDate = new DateTime(2020, 3, 1);
            LabRat.PaymentFreq = 1;
            LabRat.FloatingRateBase = 0.00287;
            LabRat.MaturityDate = TempMatDate;
            LabRat.LossMultiplier = 100;
            LabRat.PrepayMultiplier = 50;
            LabRat.Annuity = "N 5S N";
            LabRat.Interest = "5";
            LabRat.Size = 100000.0;
            Assert.AreEqual<int>(LabRat.PaymentFreq, 1, "PaymentFreq error");
            Assert.AreEqual(LabRat.FloatingRateBase, 0.00287, 0.000009, "FloatingRateBase error");
            Assert.AreEqual(LabRat.MaturityDate, TempMatDate, "MaturityDate error");
            Assert.AreEqual<int>(LabRat.LossMultiplier, 100, "LossMultiplier error");
            Assert.AreEqual<int>(LabRat.PrepayMultiplier, 50, "PrepayMultiplier error");
            Assert.AreEqual(LabRat.Annuity, "N 5S N", "Annuity error");
            Assert.AreEqual(LabRat.Interest, "5", "Interest error");
            Assert.AreEqual(LabRat.Size, 100000.0, 0.009, "Size error");

            //Test successfully ready to calculate
            Assert.AreEqual(LabRat.ReadyToCalculate().Length,0);
                
            //Test Calculate Cash flows
            LabRat.setFixedRate();
            Assert.AreEqual(LabRat.FloatingRateBase, 0, 0.000009, "Fixed rate error");
            LabRat.CalculateCashFlows("3", "2", "50", TempMatDate.AddMonths(-10));

        }

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

    }
}
