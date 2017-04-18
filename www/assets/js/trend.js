/* 
 * This file is part of the cb50_ctrl distribution (https://github.com/johnerlandsson/cb50_ctrl).
 * Copyright (c) 2017 John Erlandsson
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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
