# NYP Final Project
This project uses Iterative Closest Point(ICP) to localise an autonomous vehicle in GPS-denied environment. The use of ICP to conduct localisation is only possible, if the map of area of exploration is known to the user. The user would then be able to use the map to build a set of points for ICP to match a scan obtain though a 2D or 3D scanner, i.e. a LIDAR or a KINECT , and hence calculate the position of the vehicle.  

## Getting Started
 As both 2D and 3D scanners have a standard field of vision, by rotating and translating the scanned obtain by the sensors we would be able to obtain the position of the vehicle through the use of geometry. 
<img src="https://user-images.githubusercontent.com/20126844/34078900-ad7c1d0c-e35d-11e7-8a8e-7d7762451800.jpg" height="180">

###  Prerequisites

1) ![ICP Library](https://github.com/albusSimba/IcpLocalisation/tree/master/prerequisite)
2) ![Matrix Library](https://github.com/albusSimba/IcpLocalisation/tree/master/prerequisite)

## Known Issues

### Unexpected Reflection Matrix
It is known that the ICP library could output a ![reflection matrix](https://en.wikipedia.org/wiki/Coordinate_rotations_and_reflections) ainstead of the expected rotation matrix only. 
#### Fix
To check if ICP was giving us a reflection matrix or a rotation matrix we can easily compute the determinant of the matrix, where if the determinant is 1 its a rotation matrix and -1 if it is a reflection matrix. We would then use this detection method to compute the heading of the vehicle,θ, accordingly. For the position of the vehicle, as the position the scanned points is now reflected we will not be able use the translation given by the ICP library. The following formula is use to compute the translation of the points, where A and T can come from either a rotation or reflection matrix :

<img src="https://user-images.githubusercontent.com/20126844/34079687-6ac5a95a-e36d-11e7-9a55-45bd1ef5ebfc.jpg" height="100">

## Built With
 * The IDE used to develop this project is ![CodeGear](https://en.wikipedia.org/wiki/CodeGear). Hence some minor changes can be expected for the following code to be run other IDEs.
 
## Authors
* **Ryan Seah ** - *Initial work* - [albusSimba]( https://github.com/albusSimba)
## License
The project is free for non-commercial purposes.
