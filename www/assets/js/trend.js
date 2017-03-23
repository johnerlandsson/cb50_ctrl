angular.module("TrendApp", ["chart.js"]).controller("TrendCtrl", function($scope, $http) {

  $scope.labels = [];
  $scope.series = ['Set value', 'Process value', 'Output'];
  $scope.data = [[], [], []];

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
    }, function errorCallback(response) {
      console.log(response);
    });
  }, 1000);
});
