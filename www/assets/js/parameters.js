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

//Function for making deep copies of objects
function clone(obj) {
  var copy;

  // Handle the 3 simple types, and null or undefined
  if (null == obj || "object" != typeof obj) return obj;

  // Handle Date
  if (obj instanceof Date) {
    copy = new Date();
    copy.setTime(obj.getTime());
    return copy;
  }

  // Handle Array
  if (obj instanceof Array) {
    copy = [];
    for (var i = 0, len = obj.length; i < len; i++) {
      copy[i] = clone(obj[i]);
    }
    return copy;
  }

  // Handle Object
  if (obj instanceof Object) {
    copy = {};
    for (var attr in obj) {
      if (obj.hasOwnProperty(attr)) copy[attr] = clone(obj[attr]);
    }
    return copy;
  }

  throw new Error("Unable to copy obj! Its type isn't supported.");
}

//The module
angular.module("ParameterApp", ['ui-notification']).controller("ParameterCtrl", function($scope, $http, Notification) {
  $scope.parametersLoaded = false;
  $scope.runMsgVisible = false;
  $scope.spinnerVisible = true;
  $scope.parametersVisible = false;

  $scope.parameters = {
    regulator: {
      Kp: 0.0,
      Ki: 0.0,
      min_istate: 0.0,
      max_istate: 0.0
    }
  };
  $scope.parameters_bck = {};


  //Callback for send button
  $scope.sendParameters = function() {
    $http.post('put_parameters', {
      message: $scope.parameters
    }).then(function successCallback(response) {
      Notification.success({
        message: "Parameters written to server"
      });
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to write parameters"
      });
    });
  };

  //Callback for reset button
  $scope.resetParameters = function() {
    $scope.parameters = clone($scope.parameters_bck);
  };

  //Fetch parameters and process data from server on load
  window.onload = function() {
    //Fetch parameters
    $http.get("get_parameters").then(function successCallback(response) {
      $scope.parameters = response.data;
      $scope.parameters_bck = clone($scope.parameters);
      $scope.parametersLoaded = true;
    }, function errorCallback(response) {
      Notification.error({
        message: "Failed to receive parameters from server."
      });
    });
    // Fetch process data and check if recipe is running
    $http.get("get_pd").then(function successCallback(response) {
        if (response.data["run_recipe"]) {
          $scope.runMsgVisible = true;
          $scope.spinnerVisible = false;
        } else {
          if ($scope.parametersLoaded) {
            $scope.spinnerVisible = false;
            $scope.parametersVisible = true;
          }
        }
      },
      function errorCallback(response) {
        Notification.error({
          message: "Failed to receive process data from server."
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
