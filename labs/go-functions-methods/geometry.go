// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
	"math"
	"fmt"
	"os"
	"math/rand"
	"strconv"
	"time"
)

type Point struct{ x, y float64 }

func (p Point) X() float64{
	return p.x
}

func (p Point) Y() float64{
	return p.y
}

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}


//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	fmt.Printf("  - ")
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
			fmt.Printf(" %.2f + ", path[i-1].Distance(path[i]))
		}
	}
	fmt.Printf("%.2f", path[len(path)-1].Distance(path[0]))
	sum += path[len(path)-1].Distance(path[0])
	return sum
}


func genVertex(num int) Path {
	var path Path
	for i := 0; i < num; i++ {
		path = append(path, genPoint())
		if i == 2{
			for orienteation(path[0], path[1], path[2]) == 0{
				path[2] = genPoint()
			}
		} else if i > 2 {
			for validateVertex(path) == false {
				path[i] = genPoint()
			}
		}
	}

	return path

}

func genPoint() Point {
	rand.Seed(time.Now().UnixNano())
	point := Point {
		x: (rand.Float64() * float64((200) - 100)),
		y: (rand.Float64() * float64((200) - 100)),
	}
	return point
}

func validateVertex(path Path) bool {
	for i := 0; i < len(path); i++{
		if doIntersect(path[i%len(path)], path[(i+1)%len(path)], path[(i+2)%len(path)], path[(i+3)%len(path)]) == true{
				return false
		} 
	}

	for i := 1; i < len(path)-3; i++{
		if doIntersect(path[len(path)-1], path[0], path[(i)], path[(i+1)]) == true{
				return false
		} 
	}

	for i := 0; i < len(path)-4; i++{
		if doIntersect(path[len(path)-1], path[len(path)-2], path[(i)], path[(i+1)]) == true{
				return false
		} 
	}

	return true

}

func onSegment (p, q, r Point) bool{
	if(q.X() <= math.Max(p.X(), r.X()) && q.X() >= math.Min(p.X(), r.X()) &&
		q.Y() <= math.Max(p.Y(), r.Y()) && q.Y() >= math.Min(p.Y(), r.Y()) ) {
		return true
	}
	return false

}

func orienteation(p, q, r Point) int{
	val := ((q.Y() - p.Y()) * (r.X() - q.X()) - (q.X() - p.X()) * (r.Y() - q.Y()))
	if val == 0 {
		return 0
	}
	if val > 0 {
		return 1
	} 
	return 2
	
}

func doIntersect(p1, q1, p2, q2 Point) bool {
	o1 := orienteation(p1, q1, p2)
	o2 := orienteation(p1, q1, q2)
	o3 := orienteation(p2, q2, p1)
	o4 := orienteation(p2, q2, q1)

	if o1 != o2 && o3 != o4 {
		return true
	}

	if o1 == 0 && onSegment(p1, p2, q1){
		return true
	}
	if o2 == 0 && onSegment(p1, q2, q1){
		return true
	}
	if o3 == 0 && onSegment(p2, p1, q2){
		return true
	}
	if o4 == 0 && onSegment(p2, q1, q2){
		return true
	}

	return false

}

func main() {


	if len(os.Args) < 2{
		fmt.Printf("Missing arguments - Missing the number of points\n")
		return
	}

	input, _:= strconv.Atoi(os.Args[1])
	
	if input < 3 {
		fmt.Printf("Missing vertex - The figure needs more than 2 vertex\n")
		return
	}

	path := genVertex(input)
	
	fmt.Printf("- Generating a [%d] sides figure\n", input)
	fmt.Printf("- Figure's vertex\n")
	for i := 0; i < input; i++ {
		fmt.Printf("  - (%.2f), (%.2f)\n", path[i].X(), path[i].Y())
	}
	fmt.Printf("- Figure's Perimeter\n")
	fmt.Printf(" = %.2f\n", path.Distance())

}

//!-path
