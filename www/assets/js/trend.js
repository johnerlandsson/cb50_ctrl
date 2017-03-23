angular.module("TrendApp", ["chart.js"]).controller("TrendCtrl", function($scope, $http) {

  $scope.labels = [];
  for (var i = 0; i <= 240; i++) {
    $scope.labels.push(new Date(3600 * i));
  }
  $scope.series = ['Set value', 'Process value', 'Output'];
  $scope.data = [[], [], []];
  //$scope.data = [
    //[65, 59, 80, 81, 56, 55, 40, 56],
    //[28, 48, 40, 19, 86, 27, 90, 23],
    //[0, 20, 40, 60, 80, 100, 120, 100]
  //];

  $scope.datasetOverride = [{
    xAxisID: 'x-axis-1',
    yAxisID: 'y-axis-1'
  }];
  $scope.options = {
    scales: {
      xAxes: [{
        id: 'x-axis-1',
        type: 'time',
        display: true,
        tics: {
          autoSkip: true,
          maxTicksLimit: 20
        }
      }],
      yAxes: [{
        id: 'y-axis-1',
        type: 'linear',
        display: true,
        position: 'left'
      }]
    }
  };

  window.setInterval(function() {
    $http.get("get_trend").then(function successCallback(response) {
      $scope.labels = response.data['labels'];
      $scope.data[0] = response.data['data']['sv'];
      $scope.data[1] = response.data['data']['pv'];
      $scope.data[2] = response.data['data']['output'];
      //console.log(JSON.stringify(response.data));
      console.log($scope.data[0]);
    }, function errorCallback(response) {
      console.log(response);
    });
  }, 1000);

  //$http({
  //method: 'GET',
  //url: '/get_trend'
  //}).then(function successCallback(response) {
  //console.log("Success");
  //}, function errorCallback(response) {
  //console.log("Error");
  //});
});
