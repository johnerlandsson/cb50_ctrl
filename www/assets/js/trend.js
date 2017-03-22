'use strict';

var trendApp = angular.module("TrendApp", []);
trendApp.controller("TrendController", function($scope) {
  $scope.message = "Testing";
	console.log($scope.message);
});
