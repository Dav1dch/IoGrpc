package main

import (
	"context"
	pb "goGrpc/IoPro/service"
	"google.golang.org/grpc"
	"log"
	"time"
)

const (
	address = "localhost:50051"
	size    = "100"
	rr      = "0"
	rw      = "1"
	r       = "0"
	w       = "0"
)

func main() {
	conn, err := grpc.Dial(address, grpc.WithInsecure(), grpc.WithBlock())
	if err != nil {
		log.Fatalf("did not connect: %v", err)
	}
	defer conn.Close()
	c := pb.NewPredictClient(conn)

	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()
	r, err := c.GetPredict(ctx, &pb.PredictRequest{Size: size, Randread: rr, Randwrite: rw, Read: r, Write: w})
	if err != nil {
		log.Fatal("could not get: %v", err)
	}
	log.Printf("result: %v", r)
}
