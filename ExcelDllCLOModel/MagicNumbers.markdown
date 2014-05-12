##Magic numbers used to identify classes
When the classes are written to or read from a stream they will mark
      their identity using a "magic number"<br>
      The ID number will be a 32 bits unsigned integer, the rightmost 16 bits
      will contain the version number of the model ensuring that old versions
      will not be loaded by accident, the 16 leftmost bits represents the class
      identity<br>
      **The current model version number is: 177** (0000000010110001 in binary)
    <table style="text-align: center;" border="1" cellpadding="3">
        <tr>
          <th>Class Name</th>
          <th>Class ID</th>
          <th>Current Magic Number (binary)</th>
          <th>Current Magic Number (decimal) </th>
        </tr>
        <tr>
          <td>ManMtgCashFlows</td>
          <td>1</td>
          <td>0000000000000001 0000000010110001</td>
          <td>6571</td>
        </tr>
        <tr>
          <td>ManWatFalPrior</td>
          <td>2</td>
          <td>0000000000000010 0000000010110001</td>
          <td>131249</td>
        </tr>
		<tr>
          <td>ManMortgage</td>
          <td>3</td>
          <td>0000000000000011 0000000010110001</td>
          <td>196785</td>
        </tr>
		<tr>
          <td>ManReinvestmentTest</td>
          <td>4</td>
          <td>0000000000000100 0000000010110001</td>
          <td>262321</td>
        </tr>
		<tr>
          <td>ManTranche</td>
          <td>5</td>
          <td>0000000000000101 0000000010110001</td>
          <td>327857</td>
        </tr>
		<tr>
          <td>ManTrancheCashFlow</td>
          <td>6</td>
          <td>0000000000000110 0000000010110001</td>
          <td>393393</td>
        </tr>
		<tr>
          <td>ManWaterfall</td>
          <td>7</td>
          <td>0000000000000111 0000000010110001</td>
          <td>458929</td>
        </tr>
		<tr>
          <td>ManBloombergVector</td>
          <td>8</td>
          <td>0000000000001000 0000000010110001</td>
          <td>524465</td>
        </tr>
		<tr>
          <td>ManAnnuityVector</td>
          <td>9</td>
          <td>0000000000001001 0000000010110001</td>
          <td>590001</td>
        </tr>
		<tr>
          <td>ManBaseRateVector</td>
          <td>10</td>
          <td>0000000000001010 0000000010110001</td>
          <td>655537</td>
        </tr>
    </table>