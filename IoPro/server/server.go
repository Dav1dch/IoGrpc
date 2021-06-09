package main

import "fmt"
import "os/exec"
import "net"
import "google.golang.org/grpc"
import pb "goGrpc/IoPro/service"
import "context"
import "log"

const (
	port = ":50051"
)

type server struct {
	pb.UnimplementedPredictServer
}

func (s *server) GetPredict(ctx context.Context, in *pb.PredictRequest) (*pb.PredictReply, error) {
	log.Printf("Received: %v", in)
	return &pb.PredictReply{Message: predict(in.GetSize(), in.GetRandread(), in.GetRandwrite(), in.GetRead(), in.GetWrite())}, nil

}

func predict(size, rr, rw, r, w string) (res string) {
	cmd := exec.Command("./predict", size, rr, rw, r, w)
	output, err := cmd.Output()
	if err != nil {
		panic(err)
	}
	fmt.Printf("suggest : %v ", string(output))
	res = string(output)
	return
}

func main() {
	lis, err := net.Listen("tcp", port)
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	s := grpc.NewServer()
	pb.RegisterPredictServer(s, &server{})
	if err := s.Serve(lis); err != nil {
		log.Fatal("failed to serve: %v", err)
	}

}
