using ManagedCLO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;

namespace APIUnitTest
{
    
    
    /// <summary>
    ///This is a test class for ManWatFalPriorTest and is intended
    ///to contain all ManWatFalPriorTest Unit Tests
    ///</summary>
    [TestClass()]
    public class ManWatFalPriorTest
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


       
        [TestMethod()]
        public void TestManWatFalPrior()
        {
            // Test setting and getting properties
            ManWatFalPrior a = new ManWatFalPrior();
            a.PriorityType=ManWatFalPrior.ManWaterfallStepType.wst_OCTest;
            a.RedemptionGroup=1;
            a.RedemptionShare=0.5;
            a.GroupTarget=2;
            Assert.AreEqual< ManWatFalPrior.ManWaterfallStepType>(a.PriorityType, ManWatFalPrior.ManWaterfallStepType.wst_OCTest,"Priority type failed");
            Assert.AreEqual<int>(a.RedemptionGroup, 1,"Redemption group failed");
            Assert.AreEqual<double>(a.RedemptionShare, 0.5, "Redemption share failed");
            Assert.AreEqual<int>(a.GroupTarget, 2, "Group target failed");

            // Test File IO
            FileStream TestFileW = File.Open(TestCommons.WorkDir + "ManWatFalPriorTest.dat", FileMode.OpenOrCreate, FileAccess.Write);
            Assert.IsTrue(a.Write(TestFileW), "Writing to file failed");
            TestFileW.Close();
            ManWatFalPrior b = new ManWatFalPrior();
            FileStream TestFileR = File.Open(TestCommons.WorkDir + "ManWatFalPriorTest.dat", FileMode.Open, FileAccess.Read);
            Assert.IsTrue(b.Read(TestFileR), "Unable to read file");
            TestFileR.Close();
            File.Delete(TestCommons.WorkDir + "ManWatFalPriorTest.dat");
            Assert.AreEqual<ManWatFalPrior.ManWaterfallStepType>(a.PriorityType, b.PriorityType, "Priority type failed");
            Assert.AreEqual<int>(a.RedemptionGroup, b.RedemptionGroup, "Redemption group failed");
            Assert.AreEqual<double>(a.RedemptionShare, b.RedemptionShare, "Redemption share failed");
            Assert.AreEqual<int>(a.GroupTarget, b.GroupTarget, "Group target failed");
        }
    }
}

