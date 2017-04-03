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

//The module
angular.module("RecipeApp", ['ui-notification']).controller("RecipeCtrl", function($scope, $http, Notification) {
  $scope.pd = {};
  $scope.recipeBrowserVisible = false;
  $scope.recipeViewerVisible = false;

  // Fetch process data from server
  function get_pd() {
    $http.get("get_pd").then(function successCallback(response) {
      $scope.pd = response.data;
      //console.log($scope.pd);
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to receive process data from server."
      });
    });
  }

  function set_pump_button_bg() {
    if ($scope.pd["pump"]) {
      $scope.pump_btn_style = {
        "background-color": "rgb(102, 102, 102)"
      };
    } else {
      $scope.pump_btn_style = {
        "background-color": "rgb(153, 153, 153)"
      };
    }
  }

  function set_mixer_button_bg() {
    if ($scope.pd["mixer"]) {
      $scope.mixer_btn_style = {
        "background-color": "rgb(102, 102, 102)"
      };
    } else {
      $scope.mixer_btn_style = {
        "background-color": "rgb(153, 153, 153)"
      };
    }
  }

  function set_run_button_bg() {
    if ($scope.pd["run_recipe"]) {
      $scope.run_btn_style = {
        "background-color": "rgb(102, 102, 102)"
      };
    } else {
      $scope.run_btn_style = {
        "background-color": "rgb(153, 153, 153)"
      };
    }
  }

  // Fetch process data from server and update view accordingly
  function update_pd() {
    get_pd();

    // Set button colors
    set_pump_button_bg();
    set_mixer_button_bg();
		set_run_button_bg();

    $scope.recipeViewerVisible = $scope.pd["run_recipe"];
    $scope.recipeBrowserVisible = !$scope.recipeViewerVisible;
  }

  $scope.intervalID = window.setInterval(update_pd, 500);

  // Pump button clicked
  $scope.pump_btn_clicked = function() {
    window.clearInterval($scope.intervalID);
    $scope.pd["pump"] = !$scope.pd["pump"];
    set_pump_button_bg();
    $http.post('put_pd', {
      message: $scope.pd
    }).then(function successCallback(response) {
      $scope.pd = response.data;
      $scope.intervalID = window.setInterval(update_pd, 500);
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed send pump value"
      });
    });
  }

  // Run recipe button clicked
  $scope.run_btn_clicked = function() {
    window.clearInterval($scope.intervalID);
    $scope.pd["run_recipe"] = !$scope.pd["run_recipe"];
    $scope.recipeViewerVisible = $scope.pd["run_recipe"];
    $scope.recipeBrowserVisible = !$scope.recipeViewerVisible;
		set_run_button_bg();
    $http.post('put_pd', {
      message: $scope.pd
    }).then(function successCallback(response) {
      $scope.pd = response.data;
      $scope.intervalID = window.setInterval(update_pd, 500);
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed send run recipe value"
      });
    });
  }

  // Mixer button clicked
  $scope.mixer_btn_clicked = function() {
    window.clearInterval($scope.intervalID);
    $scope.pd["mixer"] = !$scope.pd["mixer"];
		set_mixer_button_bg();
    $http.post('put_pd', {
      message: $scope.pd
    }).then(function successCallback(response) {
      $scope.pd = response.data;
      $scope.intervalID = window.setInterval(update_pd, 500);
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed send mixer value"
      });
    });
  }
}).config(function(NotificationProvider) { //Configure notifications
  NotificationProvider.setOptions({
    delay: 10000,
    startTop: 20,
    startRight: 10,
    verticalSpacing: 20,
    horizontalSpacing: 20,
    positionX: 'left',
    positionY: 'bottom'
  });
});
