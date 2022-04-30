import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { VotingService } from '../../service/voting.service';

@Component({
  selector: 'app-cast-vote',
  templateUrl: './cast-vote.component.html',
  styleUrls: ['./cast-vote.component.scss']
})
export class CastVoteComponent implements OnInit {
  @ViewChild('input') vuid : ElementRef;
  constructor(public votingservice : VotingService) { }

  ngOnInit(): void {
  }

  allow(){
    let body = {
      "bloId" : "001",
      "boothId" : "001",
      "vuid" : this.vuid.nativeElement.value
    }
    this.votingservice.allowVote(body).then(s =>{
      console.log(s);
    })
  }

}
