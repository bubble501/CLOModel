##Magic numbers used to identify classes
When the classes are written to or read from a stream they will mark
      their identity using a "magic number"<br>
      The ID number will be a 32 bits unsigned integer, the rightmost 16 bits
      will contain the version number of the model ensuring that old versions
      will not be loaded by accident, the 16 leftmost bits represents the class
      identity<br>
      **The current model version number is: 175** (0000000010101111 in binary)
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
          <td>0000000000000001 0000000010101111</td>
          <td>65711</td>
        </tr>
        <tr>
          <td>ManWatFalPrior</td>
          <td>2</td>
          <td>000000000000001 0000000010101111</td>
          <td>131247</td>
        </tr>
		<tr>
          <td>ManMortgage</td>
          <td>3</td>
          <td>0000000000000011 0000000010101111</td>
          <td>196783</td>
        </tr>
		<tr>
          <td>ManReinvestmentTest</td>
          <td>4</td>
          <td>0000000000000100 0000000010101111</td>
          <td>262319</td>
        </tr>
		<tr>
          <td>ManTranche</td>
          <td>5</td>
          <td>0000000000000101 0000000010101111</td>
          <td>327855</td>
        </tr>
		<tr>
          <td>ManTrancheCashFlow</td>
          <td>6</td>
          <td>0000000000000110 0000000010101111</td>
          <td>393391</td>
        </tr>
		<tr>
          <td>ManWaterfall</td>
          <td>7</td>
          <td>0000000000000111 0000000010101111</td>
          <td>458927</td>
        </tr>
		<tr>
          <td>ManBloombergVector</td>
          <td>8</td>
          <td>0000000000001000 0000000010101111</td>
          <td>524463</td>
        </tr>
		<tr>
          <td>ManAnnuityVector</td>
          <td>9</td>
          <td>0000000000001001 0000000010101111</td>
          <td>589999</td>
        </tr>
    </table>