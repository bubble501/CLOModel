using ManagedCLO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;

namespace APIUnitTest
{
    
    
    /// <summary>
    ///This is a test class for ManMtgCashFlowsTest and is intended
    ///to contain all ManMtgCashFlowsTest Unit Tests
    ///</summary>
    [TestClass]
    public class ManMtgCashFlowsTest
    {
        [TestMethod]
        public void TestMtgCashFlows()
        {
            ManMtgCashFlows LabRat = new ManMtgCashFlows();
            DateTime FlowDate=new DateTime(2013,5,30);
            // Add the flows
            LabRat.AddFlow(FlowDate, 100000.0, ManMtgCashFlows.ManMtgFlowType.AccruedInterestFlow);
            LabRat.AddFlow(FlowDate.AddMonths(6), 200000.0, ManMtgCashFlows.ManMtgFlowType.AmountOutstandingFlow);
            LabRat.AddFlow(FlowDate.AddMonths(2), 300000.0, ManMtgCashFlows.ManMtgFlowType.InterestFlow);
            LabRat.AddFlow(FlowDate.AddMonths(3), 400000.0, ManMtgCashFlows.ManMtgFlowType.LossFlow);
            LabRat.AddFlow(FlowDate.AddMonths(4), 500000.0, ManMtgCashFlows.ManMtgFlowType.LossOnInterestFlow);
            LabRat.AddFlow(FlowDate.AddMonths(5), 600000.0, ManMtgCashFlows.ManMtgFlowType.PrepaymentFlow);
            LabRat.AddFlow(FlowDate.AddMonths(5), 700000.0, ManMtgCashFlows.ManMtgFlowType.PrincipalFlow);
            LabRat.AddFlow(FlowDate.AddMonths(6), 200000.0 * 0.048, ManMtgCashFlows.ManMtgFlowType.WACouponFlow);

            // Retrieve flows using dates as indexes
            String Phase = "Using Dates - ";
            double actual = LabRat.GetAccruedInterest(FlowDate);
            Assert.AreEqual(100000.0, actual, 0.009, Phase+"Adding or Retrieving accrued interest flows failed");
            actual = LabRat.GetInterest(FlowDate.AddMonths(2));
            Assert.AreEqual(300000.0, actual, 0.009, Phase + "Adding or Retrieving interest flows failed");
            actual = LabRat.GetLoss(FlowDate.AddMonths(3));
            Assert.AreEqual(400000.0, actual, 0.009, Phase + "Adding or Retrieving loss flows failed");
            actual = LabRat.GetLossOnInterest(FlowDate.AddMonths(4));
            Assert.AreEqual(500000.0, actual, 0.009, Phase + "Adding or Retrieving loss on interest flows failed");
            actual = LabRat.GetPrepay(FlowDate.AddMonths(5));
            Assert.AreEqual(600000.0, actual, 0.009, Phase + "Adding or Retrieving prepayment flows failed");
            actual = LabRat.GetScheduled(FlowDate.AddMonths(5));
            Assert.AreEqual(700000.0, actual, 0.009, Phase + "Adding or Retrieving scheduled principal flows failed");
            actual = LabRat.GetPrincipal(FlowDate.AddMonths(5));
            Assert.AreEqual(700000.0 + 600000.0, actual, 0.009, Phase + "Adding or Retrieving total principal flows failed");
            actual = LabRat.GetWAcoupon(FlowDate.AddMonths(6));
            Assert.AreEqual(0.048, actual, 0.00009, Phase + "Adding or Retrieving WA coupon flows failed");
            actual = LabRat.GetAmountOut(FlowDate.AddMonths(6));
            Assert.AreEqual(200000.0, actual, 0.009, Phase + "Adding or Retrieving amount outstanding flows failed");

            // retrieve flows using absolute indexes
            Phase = "Using Indexes - ";
            actual = LabRat.GetAccruedInterest(0);
            Assert.AreEqual(100000.0, actual, 0.009, Phase + "Adding or Retrieving accrued interest flows failed");
            actual = LabRat.GetInterest((1));
            Assert.AreEqual(300000.0, actual, 0.009, Phase + "Adding or Retrieving interest flows failed");
            actual = LabRat.GetLoss((2));
            Assert.AreEqual(400000.0, actual, 0.009, Phase + "Adding or Retrieving loss flows failed");
            actual = LabRat.GetLossOnInterest((3));
            Assert.AreEqual(500000.0, actual, 0.009, Phase + "Adding or Retrieving loss on interest flows failed");
            actual = LabRat.GetPrepay((4));
            Assert.AreEqual(600000.0, actual, 0.009, Phase + "Adding or Retrieving prepayment flows failed");
            actual = LabRat.GetScheduled((4));
            Assert.AreEqual(700000.0, actual, 0.009, Phase + "Adding or Retrieving scheduled principal flows failed");
            actual = LabRat.GetPrincipal((4));
            Assert.AreEqual(700000.0 + 600000.0, actual, 0.009, Phase + "Adding or Retrieving total principal flows failed");
            actual = LabRat.GetWAcoupon((5));
            Assert.AreEqual(0.048, actual, 0.00009, Phase + "Adding or Retrieving WA coupon flows failed");
            actual = LabRat.GetAmountOut(5);
            Assert.AreEqual(200000.0, actual, 0.009, Phase + "Adding or Retrieving amount outstanding flows failed");

            // Test finding dates
            Assert.AreEqual<int>(LabRat.FindDate(FlowDate.AddMonths(4)), 3,"Find Existing Date Failed");
            Assert.AreEqual<int>(LabRat.FindDate(FlowDate.AddMonths(7)), -1, "Find Non-Existing Date Failed");

            // Test counts
            Assert.AreEqual<int>(LabRat.Count, 6, "Wrong flows count");

            ManMtgCashFlows LabRatB = new ManMtgCashFlows();

            //Test adding flows from another instance
            LabRatB.AddFlow(LabRat);
            Phase = "Using Copied Instance - ";
            actual = LabRatB.GetAccruedInterest(0);
            Assert.AreEqual(100000.0, actual, 0.009, Phase + "Adding or Retrieving accrued interest flows failed");
            actual = LabRatB.GetInterest((1));
            Assert.AreEqual(300000.0, actual, 0.009, Phase + "Adding or Retrieving interest flows failed");
            actual = LabRatB.GetLoss((2));
            Assert.AreEqual(400000.0, actual, 0.009, Phase + "Adding or Retrieving loss flows failed");
            actual = LabRatB.GetLossOnInterest((3));
            Assert.AreEqual(500000.0, actual, 0.009, Phase + "Adding or Retrieving loss on interest flows failed");
            actual = LabRatB.GetPrepay((4));
            Assert.AreEqual(600000.0, actual, 0.009, Phase + "Adding or Retrieving prepayment flows failed");
            actual = LabRatB.GetScheduled((4));
            Assert.AreEqual(700000.0, actual, 0.009, Phase + "Adding or Retrieving scheduled principal flows failed");
            actual = LabRatB.GetPrincipal((4));
            Assert.AreEqual(700000.0 + 600000.0, actual, 0.009, Phase + "Adding or Retrieving total principal flows failed");
            actual = LabRatB.GetWAcoupon((5));
            Assert.AreEqual(0.048, actual, 0.00009, Phase + "Adding or Retrieving WA coupon flows failed");
            actual = LabRatB.GetAmountOut(5);
            Assert.AreEqual(200000.0, actual, 0.009, Phase + "Adding or Retrieving amount outstanding flows failed");
            Assert.AreEqual<int>(LabRatB.Count, 6, Phase + "Wrong flows count");

            //Test removing flows
            LabRatB.RemoveAllFlows();
            Assert.AreEqual<int>(LabRatB.Count, 0,"Reset Flows Failed");

            //Test Reading and Writing to file
            FileStream TestFileW = File.Open(TestCommons.WorkDir + "ManMtgCashFlowsTest.dat", FileMode.OpenOrCreate, FileAccess.Write);
            Assert.IsTrue(LabRat.Write(TestFileW), "Writing to file failed");
            TestFileW.Close();
            FileStream TestFileR = File.Open(TestCommons.WorkDir + "ManMtgCashFlowsTest.dat", FileMode.Open, FileAccess.Read);
            Assert.IsTrue(LabRatB.Read(TestFileR), "Unable to read file");
            TestFileR.Close();
            File.Delete(TestCommons.WorkDir + "ManMtgCashFlowsTest.dat");
            Phase = "Using File Loaded Instance - ";
            actual = LabRatB.GetAccruedInterest(0);
            Assert.AreEqual(100000.0, actual, 0.009, Phase + "Adding or Retrieving accrued interest flows failed");
            actual = LabRatB.GetInterest((1));
            Assert.AreEqual(300000.0, actual, 0.009, Phase + "Adding or Retrieving interest flows failed");
            actual = LabRatB.GetLoss((2));
            Assert.AreEqual(400000.0, actual, 0.009, Phase + "Adding or Retrieving loss flows failed");
            actual = LabRatB.GetLossOnInterest((3));
            Assert.AreEqual(500000.0, actual, 0.009, Phase + "Adding or Retrieving loss on interest flows failed");
            actual = LabRatB.GetPrepay((4));
            Assert.AreEqual(600000.0, actual, 0.009, Phase + "Adding or Retrieving prepayment flows failed");
            actual = LabRatB.GetScheduled((4));
            Assert.AreEqual(700000.0, actual, 0.009, Phase + "Adding or Retrieving scheduled principal flows failed");
            actual = LabRatB.GetPrincipal((4));
            Assert.AreEqual(700000.0 + 600000.0, actual, 0.009, Phase + "Adding or Retrieving total principal flows failed");
            actual = LabRatB.GetWAcoupon((5));
            Assert.AreEqual(0.048, actual, 0.00009, Phase + "Adding or Retrieving WA coupon flows failed");
            actual = LabRatB.GetAmountOut(5);
            Assert.AreEqual(200000.0, actual, 0.009, Phase + "Adding or Retrieving amount outstanding flows failed");
            Assert.AreEqual<int>(LabRatB.Count, 6, Phase + "Wrong flows count");
            
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
