##Magic numbers used to identify classes
When the classes are written to or read from a stream they will mark
      their identity using a "magic number"<br>
      The ID number will be a 32 bits unsigned integer, the rightmost 16 bits
      will contain the version number of the model ensuring that old versions
      will not be loaded by accident, the 16 leftmost bits represents the class
      identity<br>
      **The current model version number is: 160** (10100000 in binary)
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
          <td>00000001 10100000</td>
          <td>416</td>
        </tr>
        <tr>
          <td>ManWatFalPrior</td>
          <td>2</td>
          <td>00000010 10100000</td>
          <td>672</td>
        </tr>
		<tr>
          <td>ManMortgage</td>
          <td>3</td>
          <td>00000011 10100000</td>
          <td>928</td>
        </tr>
		<tr>
          <td>ManReinvestmentTest</td>
          <td>4</td>
          <td>00000100 10100000</td>
          <td>1184</td>
        </tr>
		<tr>
          <td>ManTranche</td>
          <td>5</td>
          <td>00000101 10100000</td>
          <td>1440</td>
        </tr>
		<tr>
          <td>ManTrancheCashFlow</td>
          <td>6</td>
          <td>00000110 10100000</td>
          <td>1696</td>
        </tr>
		<tr>
          <td>ManWaterfall</td>
          <td>7</td>
          <td>00000111 10100000</td>
          <td>1952</td>
        </tr>
    </table>