import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';
import { VotingService } from '../../service/voting.service';

@Component({
  selector: 'app-main',
  templateUrl: './main.component.html',
  styleUrls: ['./main.component.scss']
})
export class MainComponent implements OnInit {

  constructor(public votingservice : VotingService, private router : Router) { }

  ngOnInit(): void {
  }

  castVote(){
    this.router.navigate(["/cast-vote"])
  }

  registerVoter(){
    this.router.navigate(["/register-voter"]);
  }



}
