/*function doPost(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var data = JSON.parse(e.postData.contents);

  sheet.appendRow([
    new Date(),
    data.device,
    data.temperature,
    data.humidity
  ]);

  return ContentService.createTextOutput("OK");
}

function doGet(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var lastRow = sheet.getLastRow();
  
  // Get last data row (Temperature & Humidity)
  var device = sheet.getRange(lastRow, 2).getValue();
  var temperature = sheet.getRange(lastRow, 3).getValue();
  var humidity = sheet.getRange(lastRow, 4).getValue();

  var output = {
    device: device,
    temperature: temperature,
    humidity: humidity
  };

  return ContentService
    .createTextOutput(JSON.stringify(output))
    .setMimeType(ContentService.MimeType.JSON);
}*/



function toBanglaNumber(num) {
  var en = ['0','1','2','3','4','5','6','7','8','9'];
  var bn = ['০','১','২','৩','৪','৫','৬','৭','৮','৯'];
  var str = num.toString();
  for (var i = 0; i < en.length; i++) {
    var re = new RegExp(en[i], 'g');
    str = str.replace(re, bn[i]);
  }
  return str;
}

function doPost(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var data = JSON.parse(e.postData.contents);

  var days = ["রবিবার", "সোমবার", "মঙ্গলবার", "বুধবার", "বৃহস্পতিবার", "শুক্রবার", "শনিবার"];
  var months = ["জানুয়ারি", "ফেব্রুয়ারি", "মার্চ", "এপ্রিল", "মে", "জুন",
                "জুলাই", "আগস্ট", "সেপ্টেম্বর", "অক্টোবর", "নভেম্বর", "ডিসেম্বর"];

  var now = new Date();
  var dayName = days[now.getDay()];
  var dateNum = toBanglaNumber(now.getDate());
  var monthName = months[now.getMonth()];
  var year = toBanglaNumber(now.getFullYear());

  var hours = now.getHours();
  var minutes = now.getMinutes();
  var seconds = now.getSeconds();

  var ampm = hours >= 12 ? "PM" : "AM";
  hours = hours % 12;
  hours = hours ? hours : 12; // 0 হলে 12 হবে
  hours = toBanglaNumber(hours);
  minutes = toBanglaNumber(minutes < 10 ? "0" + minutes : minutes);
  seconds = toBanglaNumber(seconds < 10 ? "0" + seconds : seconds);

  var timestamp = dayName + ", " + dateNum + " " + monthName + " " + year + " at " +
                  hours + ":" + minutes + ":" + seconds + " " + ampm;

  sheet.appendRow([
    timestamp,
    data.device,
    data.temperature,
    data.humidity
  ]);

  return ContentService.createTextOutput("OK");
}

function doGet(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var f1Value = sheet.getRange("F1").getValue();
  return ContentService.createTextOutput(f1Value.toString());
}
