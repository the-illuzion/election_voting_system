import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { environment } from '../../environments/environment';

@Injectable({
  providedIn: 'root'
})
export class VotingService {

  constructor(private http: HttpClient) { }

  login(body: any){
    return new Promise((resolve, reject) => {
      this.http.post(environment.apiUrl + 'BLO/login', body)
        .subscribe((success: any) => {
          return resolve(success);
        }, error => {
          return resolve(error);
        });
    });
  }

  allowVote(body: any){
    return new Promise((resolve, reject) => {
      this.http.post(environment.apiUrl + 'blo/allowVote', body)
        .subscribe((success: any) => {
          return resolve(success);
        }, error => {
          return resolve(error);
        });
    });
  }

  registerVote(body){
    return new Promise((resolve, reject) => {
      this.http.post(environment.apiUrl + 'blo/registerVoter', body)
        .subscribe((success: any) => {
          return resolve(success);
        }, error => {
          return resolve(error);
        });
    });
  }

  sendVoterData(body){
    return new Promise((resolve, reject) => {
      this.http.post(environment.apiUrl + 'voter/register', body)
        .subscribe((success: any) => {
          return resolve(success);
        }, error => {
          return resolve(error);
        });
    });
  }

  updateMachineStatus(body){
    return new Promise((resolve, reject) => {
      this.http.post(environment.apiUrl + 'voter/register', body)
        .subscribe((success: any) => {
          return resolve(success);
        }, error => {
          return resolve(error);
        });
    });
  }



}
